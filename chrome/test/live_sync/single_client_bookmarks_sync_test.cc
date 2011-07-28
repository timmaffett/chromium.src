// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/sync/profile_sync_service_harness.h"
#include "chrome/test/live_sync/bookmarks_helper.h"
#include "chrome/test/live_sync/live_sync_test.h"

class SingleClientBookmarksSyncTest : public LiveSyncTest {
 public:
  SingleClientBookmarksSyncTest() : LiveSyncTest(SINGLE_CLIENT) {}
  virtual ~SingleClientBookmarksSyncTest() {}

 private:
  DISALLOW_COPY_AND_ASSIGN(SingleClientBookmarksSyncTest);
};

IN_PROC_BROWSER_TEST_F(SingleClientBookmarksSyncTest, OfflineToOnline) {
  ASSERT_TRUE(SetupSync()) << "SetupSync() failed.";

  DisableNetwork(GetProfile(0));
  const BookmarkNode* node = BookmarksHelper::AddFolder(0, L"title");
  BookmarksHelper::SetTitle(0, node, L"new_title");
  ASSERT_FALSE(GetClient(0)->AwaitSyncCycleCompletion("Offline state change."));
  ASSERT_EQ(ProfileSyncService::Status::OFFLINE_UNSYNCED,
            GetClient(0)->GetStatus().summary);

  EnableNetwork(GetProfile(0));
  ASSERT_TRUE(GetClient(0)->AwaitSyncCycleCompletion("Commit changes."));
  ASSERT_EQ(ProfileSyncService::Status::READY,
            GetClient(0)->GetStatus().summary);
  ASSERT_TRUE(BookmarksHelper::ModelMatchesVerifier(0));
}

IN_PROC_BROWSER_TEST_F(SingleClientBookmarksSyncTest, Sanity) {
  ASSERT_TRUE(SetupClients()) << "SetupClients() failed.";

  // Starting state:
  // other_node
  //    -> top
  //      -> tier1_a
  //        -> http://mail.google.com  "tier1_a_url0"
  //        -> http://www.pandora.com  "tier1_a_url1"
  //        -> http://www.facebook.com "tier1_a_url2"
  //      -> tier1_b
  //        -> http://www.nhl.com "tier1_b_url0"
  const BookmarkNode* top = BookmarksHelper::AddFolder(
      0, BookmarksHelper::GetOtherNode(0), 0, L"top");
  const BookmarkNode* tier1_a = BookmarksHelper::AddFolder(
      0, top, 0, L"tier1_a");
  const BookmarkNode* tier1_b = BookmarksHelper::AddFolder(
      0, top, 1, L"tier1_b");
  const BookmarkNode* tier1_a_url0 = BookmarksHelper::AddURL(
      0, tier1_a, 0, L"tier1_a_url0", GURL("http://mail.google.com"));
  const BookmarkNode* tier1_a_url1 = BookmarksHelper::AddURL(
      0, tier1_a, 1, L"tier1_a_url1", GURL("http://www.pandora.com"));
  const BookmarkNode* tier1_a_url2 = BookmarksHelper::AddURL(
      0, tier1_a, 2, L"tier1_a_url2", GURL("http://www.facebook.com"));
  const BookmarkNode* tier1_b_url0 = BookmarksHelper::AddURL(
      0, tier1_b, 0, L"tier1_b_url0", GURL("http://www.nhl.com"));

  ASSERT_TRUE(SetupSync()) << "SetupSync() failed.";
  ASSERT_TRUE(GetClient(0)->AwaitSyncCycleCompletion(
      "Waiting for initial sync completed."));
  ASSERT_TRUE(BookmarksHelper::ModelMatchesVerifier(0));

  //  Ultimately we want to end up with the following model; but this test is
  //  more about the journey than the destination.
  //
  //  bookmark_bar
  //    -> CNN (www.cnn.com)
  //    -> tier1_a
  //      -> tier1_a_url2 (www.facebook.com)
  //      -> tier1_a_url1 (www.pandora.com)
  //    -> Porsche (www.porsche.com)
  //    -> Bank of America (www.bankofamerica.com)
  //    -> Seattle Bubble
  //  other_node
  //    -> top
  //      -> tier1_b
  //        -> Wired News (www.wired.com)
  //        -> tier2_b
  //          -> tier1_b_url0
  //          -> tier3_b
  //            -> Toronto Maple Leafs (mapleleafs.nhl.com)
  //            -> Wynn (www.wynnlasvegas.com)
  //      -> tier1_a_url0
  const BookmarkNode* bar = BookmarksHelper::GetBookmarkBarNode(0);
  const BookmarkNode* cnn = BookmarksHelper::AddURL(0, bar, 0, L"CNN",
      GURL("http://www.cnn.com"));
  ASSERT_TRUE(cnn != NULL);
  BookmarksHelper::Move(0, tier1_a, bar, 1);
  ASSERT_TRUE(GetClient(0)->AwaitSyncCycleCompletion("Bookmark moved."));
  ASSERT_TRUE(BookmarksHelper::ModelMatchesVerifier(0));

  const BookmarkNode* porsche = BookmarksHelper::AddURL(0, bar, 2, L"Porsche",
      GURL("http://www.porsche.com"));
  // Rearrange stuff in tier1_a.
  ASSERT_EQ(tier1_a, tier1_a_url2->parent());
  ASSERT_EQ(tier1_a, tier1_a_url1->parent());
  BookmarksHelper::Move(0, tier1_a_url2, tier1_a, 0);
  BookmarksHelper::Move(0, tier1_a_url1, tier1_a, 2);
  ASSERT_TRUE(GetClient(0)->AwaitSyncCycleCompletion(
      "Rearrange stuff in tier1_a"));
  ASSERT_TRUE(BookmarksHelper::ModelMatchesVerifier(0));

  ASSERT_EQ(1, tier1_a_url0->parent()->GetIndexOf(tier1_a_url0));
  BookmarksHelper::Move(0, tier1_a_url0, bar, bar->child_count());
  const BookmarkNode* boa = BookmarksHelper::AddURL(0, bar, bar->child_count(),
      L"Bank of America", GURL("https://www.bankofamerica.com"));
  ASSERT_TRUE(boa != NULL);
  BookmarksHelper::Move(0, tier1_a_url0, top, top->child_count());
  const BookmarkNode* bubble = BookmarksHelper::AddURL(
      0, bar, bar->child_count(), L"Seattle Bubble",
          GURL("http://seattlebubble.com"));
  ASSERT_TRUE(bubble != NULL);
  const BookmarkNode* wired = BookmarksHelper::AddURL(0, bar, 2, L"Wired News",
      GURL("http://www.wired.com"));
  const BookmarkNode* tier2_b = BookmarksHelper::AddFolder(
      0, tier1_b, 0, L"tier2_b");
  BookmarksHelper::Move(0, tier1_b_url0, tier2_b, 0);
  BookmarksHelper::Move(0, porsche, bar, 0);
  BookmarksHelper::SetTitle(0, wired, L"News Wired");
  BookmarksHelper::SetTitle(0, porsche, L"ICanHazPorsche?");
  ASSERT_TRUE(GetClient(0)->AwaitSyncCycleCompletion("Change title."));
  ASSERT_TRUE(BookmarksHelper::ModelMatchesVerifier(0));

  ASSERT_EQ(tier1_a_url0->id(), top->GetChild(top->child_count() - 1)->id());
  BookmarksHelper::Remove(0, top, top->child_count() - 1);
  BookmarksHelper::Move(0, wired, tier1_b, 0);
  BookmarksHelper::Move(0, porsche, bar, 3);
  const BookmarkNode* tier3_b = BookmarksHelper::AddFolder(
      0, tier2_b, 1, L"tier3_b");
  const BookmarkNode* leafs = BookmarksHelper::AddURL(
      0, tier1_a, 0, L"Toronto Maple Leafs", GURL("http://mapleleafs.nhl.com"));
  const BookmarkNode* wynn = BookmarksHelper::AddURL(0, bar, 1, L"Wynn",
      GURL("http://www.wynnlasvegas.com"));

  BookmarksHelper::Move(0, wynn, tier3_b, 0);
  BookmarksHelper::Move(0, leafs, tier3_b, 0);
  ASSERT_TRUE(GetClient(0)->AwaitSyncCycleCompletion(
      "Move after addition of bookmarks."));
  ASSERT_TRUE(BookmarksHelper::ModelMatchesVerifier(0));
}

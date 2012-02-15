#ifndef HTML_H
#define HTML_H

#include <iostream>
#include "../track.h"
#include "../art.h"
#include "../user.h"
#include "../comment.h"

extern std::string path;

//! Page generation
namespace Html{

// escape.cpp
std::string escape(const std::string &);
std::string escapeEmail(const std::string &);
std::string format(std::string);

// page.cpp
std::string header(const std::string &title=std::string(), const std::string &head=std::string(), int status=200);
std::string atomFeed(const std::string &url);
std::string footer();
std::string quickStart();
std::string notFound(const std::string &what="Page");
std::string errorPage(const std::string &err);

enum TrackList {
    Standard,
    //! Artist not displayed.
    Compact,
};

// track.cpp
std::string uploadForm(const std::string &action);
std::string trackPage(int tid);
std::string embedTrack(int tid);
std::string embedTrackCode(const Track &t, int w=0);
std::string trackList(const std::vector<Track> &tracks, TrackList l=Standard);
std::string latestTracks(int n);
std::string tracksPage(const std::string &title, const std::vector<Track> &tracks);
std::string trackSearch(const std::string &q);
std::string category(int cid);
std::string downloadTrack(int tid, Track::Format f);
std::string trackArt(int tid, Art::Size sz=Art::Full);

// feed.cpp
std::string feedIcon(const std::string &url);

// user.cpp
std::string userList(const std::vector<User> &users);
std::string userPage(int uid);
std::string userSearch(const std::string &q);
std::string usersPage();
std::string artistsPage();
std::string searchForm(const std::string &action, const std::string &q=std::string());
std::string favorites(int uid);

// news.cpp
std::string newsPage(int nid);
std::string latestNews(int n);

// comment.cpp
std::string comments(const std::vector<Comment> &cmts, const std::string title="Comments");
std::string commentForm(const std::string &action);

// contest.cpp
std::string contest(int cid, const std::string &host);

// fac.cpp
std::string faq();

// credits.cpp
std::string credits();

// home.cpp
std::string home();

}

#endif // HTML_H
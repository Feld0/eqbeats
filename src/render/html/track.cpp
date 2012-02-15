#include "../html.h"
#include "../http.h"
#include "../../session.h"
#include "../../utils.h"
#include <sstream>
#include <algorithm>

using namespace std;

string renameForm(const Track &t){
    return
        "<form method=\"post\" action=\"" + t.url() + "/rename\">"
            "<input type=\"text\" name=\"title\" value=\"" + Html::escape(t.title()) + "\" />"
            "<input type=\"submit\" value=\"Rename\" />"
        "</form>";
}

string Html::uploadForm(const std::string &action){
    stringstream s;
    s << "<div id=\"track-uploader\">"
            "<form action=\"" << action << "\" method=\"post\" enctype=\"multipart/form-data\" id=\"track-uploader-form\">"
                "<input type=\"file\" accept=\"audio/mpeg\" name=\"file\" />"
                "<input type=\"submit\" value=\"Upload an MP3\" />"
            "</form>"
        "</div>"
        "<script src=\"/static/fileuploader.js\" type=\"text/javascript\"></script>"
        "<script>"
            "function createUploader(){"
                "var uploader = new qq.FileUploader({"
                    "element: document.getElementById('track-uploader'),"
                    "allowedExtensions: ['mp3'],"
                    "action: '" << action << "'"
                "});"
            "}"
            "window.onload = createUploader;"
        "</script>";
    return s.str();
}

string artUploadForm(const Track &t){
    return
        "<form action=\"" + t.url() + "/art/upload\" method=\"post\" enctype=\"multipart/form-data\">"
            "<input type=\"file\" name=\"file\" /><br />"
            "<input type=\"submit\" value=\"Upload a picture\" />"
        "</form>";
}

string publishForm(const Track &t){
    return
        "<form class=\"publish\" action=\"" + t.url() + "/publish\" method=\"post\">"
          "This track is not yet published. "
          " <input type=\"submit\" value=\"Publish\"/>"
          "<input type=\"hidden\" name=\"tid\" value=\"" + number(t.id()) + "\"/>"
      "</form>";
}

string player(const Track &t){
    return
        "<div id=\"player\"></div>"
        "<script src=\"/static/sm2.js\"></script>"
        "<script>"
            "var mp3path = \"" + t.url(Track::MP3) +"\";"
            "var oggpath = \"" + t.url(Track::Vorbis) +"\";"
        "</script>"
        "<script src=\"/static/player.js\"></script>"
        "<noscript>"
            "<audio controls>"
                "<source type=\"audio/ogg\" src=\"" + t.url(Track::Vorbis) + "\" />"
                "<source type=\"audio/mpeg\" src=\"" + t.url(Track::MP3) + "\" />"
            "</audio>"
        "</noscript>";
}

string Html::embedTrackCode(const Track &t, int w){
    if(!w) w = 500;
    return
        "<iframe width=\""+number(w)+"px\" height=\"150px\" frameborder=\"0\" src=\"" + eqbeatsUrl() + t.url() + "/embed\">"
            "<a href=\"" + eqbeatsUrl() + t.url() + "\" target=\"_blank\">" + Html::escape(Html::escape(t.title())) + "</a>"
            " by <a href=\"" + eqbeatsUrl() + User::url(t.artistId()) + "\" target=\"_blank\">" + Html::escape(t.artist()) + "</a>"
        "</iframe>";
}

string embedCode(const Track &t){
    return "<textarea id=\"embedcode\">" + Html::embedTrackCode(t) + "</textarea>"
      "<script>document.getElementById('embedcode').style.display='none';</script>";
}

string cats(const Track &t, bool edition){
    vector<Category> cs = t.getCategories();
    if(cs.empty() && !edition) return string();
    stringstream s;
    s << "<div class=\"toolbar cats\"><img src=\"/static/tag.png\" /> Categories:";
    if(edition)
        s << "<form action=\"" << t.url() << "/cat\" method=\"post\">";
    for(vector<Category>::const_iterator i=cs.begin(); i!=cs.end(); i++){
        if(edition)
            s << " <input type=\"checkbox\" name=\"" << i->id() << "\" />";
        s << " <a href=\"" << i->url() << "\">" << i->name() << "</a>";
    }
    if(edition){
        if(!cs.empty())
            s << " <input type=\"submit\" name=\"rmcats\" value=\"Remove selected\" />";
        s << " <select name=\"cat\">";
        vector<Category> all = Category::list();
        for(vector<Category>::const_iterator i=all.begin(); i!=all.end(); i++){
            if(find(cs.begin(),cs.end(),*i) == cs.end())
                s << "<option value=\"" << i->id() << "\">" << i->name() << "</option>";
        }
        s << "</select>"
             "<input type=\"submit\" name=\"addcat\" value=\"Add\" />"
             "</form>";
    }
    s << "</div>";
    return s.str();
}

string favStar(const Track &t){
    bool isFav = Session::user().isFavorite(t.id());
    return (string)
        "<a href=\"" + (Session::user() ? "" : "/login?redirect=") + t.url() + "/" + (isFav?"un":"") + "favorite\""
          " title=\"" + (isFav?"Remove from favorites":"Add to favorites") + "\">"
        "<img src=\"/static/star" + (isFav?"":"-empty") + ".png\" /></a>";
}

string Html::trackPage(int tid){
    Track t(tid);
    if(!t) return notFound("Track");
    stringstream s;
    bool edition = Session::user().id() == t.artistId();
    Art art(tid);
    if(!edition)
        t.hit();
    s << header(t.title(),
            "<link rel=\"alternate\" type=\"application/json+oembed\" href=\"" + eqbeatsUrl() + "/oembed?url=http%3A//eqbeats.org" + path + "&format=json\">"
            "<link rel=\"alternate\" type=\"text/xml+oembed\" href=\"" + eqbeatsUrl() + "/oembed?url=http%3A//eqbeats.org" + path + "&format=xml\">")
      << "<div class=\"track\">"
         << "<h2>" << escape(t.title()) << " " << favStar(t) << "</h2>"
            "<h4>by <a href=\"" << User::url(t.artistId()) <<  "\">"
                    << escape(t.artist()) << "</a></h3>"
      << (art?"<img class=\"art\" src=\"" + art.url(Art::Medium) + "\" />":"")
      << player(t)
      << "<div class=\"toolbar\">"
         "<span><img src=\"/static/drive-download.png\" /> Download : "
         " <a href=\"" << t.url(Track::Vorbis) << "\">OGG Vorbis</a>"
         " <a href=\"" << t.url(Track::MP3) << "\">MP3</a>"
      << (art?" <a href=\"" + art.url() + "\" target=\"_blank\">Art</a>":"")
      << "</span> <span><img src=\"/static/balloon-white-left.png\" alt=\"Share\" /> Share : <a href=\"#embedcode\" onclick=\"document.getElementById('embedcode').style.display='block';return false;\">Embed</a></span>";
    if(edition)
        s << " <span><img src=\"/static/edit-number.png\" /> Hits : " << t.getHits()
          << "</span> <span><img src=\"/static/star.png\" /> Favourites : " << t.favoritesCount() << "</span>";
    s << "</div>" << embedCode(t);
    string notes = t.getNotes();
    if(!notes.empty())
        s << "<div class=\"notes\">" << format(notes) << "</div>";
    s << cats(t, edition);
    if(edition)
        s << "<h3><img src=\"/static/pencil.png\" /> Edit</h3>"
             "<div class=\"edit\">"
          << (t.visible()?"":publishForm(t))
          << "<div class=\"column\">"
          << "<h4>Rename</h4>" << renameForm(t)
          << "<h4>Re-upload</h4>" << uploadForm(t.url()+"/upload")
          << "<h4>Art</h4>" << artUploadForm(t)
          << "</div>"
          << "<div class=\"column\">"
             "<h4>Notes</h4>"
             "<form action=\"" << t.url() << "/notes\" method=\"post\">"
                 "<textarea name=\"notes\">" << escape(notes) << "</textarea><br />"
                 "<input type=\"submit\" value=\"Update description\" />"
             "</form>"
             "</div>"
             "<a class=\"delete\" href=\"" << t.url() << "/delete\">Delete track</a>"
             "<div style=\"clear:both;\"></div>"
             "</div>";
    s << comments(Comment::forTrack(t.id()))
      << commentForm(t.url() + "/comment")
      << "</div>"
      << footer();
    return s.str();
}

string Html::embedTrack(int tid){
    Track t(tid);
    Art art(tid);
    stringstream s;
    s << Http::header("text/html", t?200:404)
      << "<html><head>"
            "<title>" << (t ? escape(t.title()) : "Track not found") << " - Equestrian Beats</title>"
            "<link rel=\"stylesheet\" type=\"text/css\" href=\"/static/player.css\" />"
        "</head><body><div id=\"player-embed\">"
        "<a href=\"" + t.url() + "\" target=\"_blank\" class=\"pic\">"
        + (art?
            "<img src=\"" + art.url(Art::Thumbnail) + "\" class=\"cover\" alt=\"Cover\"  />":
            "<img src=\"/static/logo.png\" class=\"logo\" alt=\"Equestrian Beats\" />"
        ) + "</a>";
    if(t)
        s << "<h3><a href=\"" + t.url() + "\" target=\"_blank\">" + escape(t.title()) + "</a></h3>"
             "<h4>by <a href=\"" << User::url(t.artistId()) <<  "\" target=\"_blank\">" << escape(t.artist()) << "</a></h4>"
          << player(t);
    else s << "<h3 style=\"margin-bottom:10px;\">Track not found</h3>";
    s << "</div></body></html>";
    return s.str();
}

string Html::trackList(const vector<Track> &tracks, Html::TrackList l){
    if(tracks.empty()) return "<div class=\"empty tracklist\">Nothing here yet.</div>";
    stringstream s;
    s << "<ul class=\"tracklist\">";
    for(vector<Track>::const_iterator i=tracks.begin(); i!=tracks.end(); i++){
        s << "<li";
        if(!i->visible()) s << " class=\"hidden\"";
        s << "><a href=\"" << i->url() << "\">" << escape(i->title()) << "</a>";
        if(l != Compact)
            s << " <span class=\"by\">by <a href=\"" << User::url(i->artistId()) << "\">"
              << escape(i->artist()) << "</a></span>";
        s << "</li>";
    }
    s << "</ul>";
    return s.str();
}

string Html::tracksPage(const string &title, const vector<Track> &tracks){
    return header(title)
         + "<h2>" + title + "</h2>"
         + trackList(tracks)
         + footer();
}

string Html::latestTracks(int n){
    return header("Latest tracks", atomFeed("/tracks/latest/atom"))
         + "<h2>Latest tracks " + feedIcon("/tracks/latest/atom") + "</h2>"
         + trackList(Track::latest(n))
         + footer();
}

string Html::trackSearch(const std::string &q){
    stringstream s;
    s << header("Track search")
      << "<h2>Track search</h2>"
      << searchForm("/tracks/search", q);
    if(!q.empty()){
        vector<Track> res = Track::search(q);
        if(res.empty())
            s << "No result.";
        else
            s << trackList(res);
    }
    s << footer();
    return s.str();
}

string Html::category(int cid){
    Category c(cid);
    if(!c) return notFound("Category");
    return header(c.name(), atomFeed(c.url()+"/atom"))
         + "<h2>" + c.name() + "</h2>"
         + trackList(Track::byCategory(cid))
         + footer();
}
string Html::downloadTrack(int tid, Track::Format f){
    Track t(tid);
    if(!t) return notFound("Track");
    string ext = f == Track::Vorbis ? ".ogg" : ".mp3";
    string mime = f == Track::Vorbis ? "ogg" : "mpeg";
    return Http::download("/downloads/tracks/"+number(tid)+ext, t.artist()+" - "+t.title()+ext, "audio/"+mime, true);
}

string Html::trackArt(int tid, Art::Size sz){
    Art art(tid);
    if(!art) return notFound("Art");
    Track t(tid);
    if(!t) return notFound("Track");
    Art::Format f = art.getFormat();
    string ext = f == Art::PNG ? ".png" : f == Art::JPEG ? ".jpg" : f == Art::GIF ? ".gif" : "";
    string mime = f == Art::PNG ? "image/png" : f == Art::JPEG ? "image/jpeg" : f == Art::GIF ? "image/gif" : "application/octet-stream";
    return Http::download((string)"/downloads/art/"+(sz==Art::Medium?"medium/":sz==Art::Thumbnail?"thumb/":"")+number(tid), t.artist()+" - "+t.title()+ext, mime);
}
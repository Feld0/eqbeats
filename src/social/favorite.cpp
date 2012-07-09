#include "actions.h"
#include "../session.h"
#include "../track.h"
#include <text/text.h>
#include "../path.h"
#include "../follower.h"
#include "../contest.h"
#include "../cgi.h"
#include "../event.h"
#include <pcrecpp.h>

bool fromEqBeats(){
    return pcrecpp::RE("http://(www\\.)?" + cgi.getEnvironment().getServerName() + "/.*", pcrecpp::RE_Options().set_caseless(true)).FullMatch(cgi.getEnvironment().getReferrer());
}

void Action::follow(int uid, bool add){
    if(!fromEqBeats()) return; // Http::redirect(User::url(uid));
    User u(uid);
    if(!u);
    else if(!Session::user())
        ;//Http::redirect("/login?redirect=" + u.url() + "/" + (add?"":"un") + "follow");
    else if(Session::user() == u)
        ;//Http::redirect(u.url());
    else{
        if(add){
            Event::follow(u, Session::user());
            Follower(Session::user()).follow(uid);
        }
        else
            Follower(Session::user()).unfollow(uid);
        //Http::redirect(u.url());
    }
}

void Action::favorite(int tid, bool add){
    if(!fromEqBeats()) return; // Http::redirect(Track::url(tid));
    Track t(tid);
    if(!t);
    else if(!Session::user())
        ;//Http::redirect("/login?redirect=" + t.url() + "/" + (add?"":"un") + "favorite");
    else{
        if(add){
            if(Follower(Session::user()).addToFavorites(tid))
                Event::favorite(t, Session::user());
        }
        else
            Follower(Session::user()).removeFromFavorites(tid);
        //Http::redirect(t.url());
    }
}

void Action::vote(int cid){
    if(!fromEqBeats()) return; // Http::redirect(Contest::url(cid));
    Contest c(cid);
    if(!c) return;
    std::string param = cgi("tid");
    if(!param.empty()){
        int tid = number(param.substr(1));
        if(!tid);
        else if(param[0] == '-')
            c.unvote(tid);
        else c.vote(tid);
    }
    //Http::redirect(c.url()+"#submissions");
}
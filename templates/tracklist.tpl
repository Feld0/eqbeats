{{%AUTOESCAPE context="HTML"}}

<ul class="tracklist">

    {{#EMPTY}}
    <li class="empty">Nothing here yet</li>
    {{/EMPTY}}

    {{#TRACK}}
    <li {{#IS_HIDDEN}}class="hidden"{{/IS_HIDDEN}} onclick="toggle(this.getElementsByClassName('player')[0]);">
        {{#HAS_COVER}}<img class="cover" alt="" src="/track/{{TID}}/art/thumb" />{{/HAS_COVER}}
        <div class="title"><a href="/track/{{TID}}" title="{{TITLE}}">{{TITLE}}</a></div>
        <div class="by">by <a href="/user/{{UID}}">{{USERNAME}}</a></div>
        <div style="clear:both;"></div>
        {{>PLAYER}}
    </li>
    {{/TRACK}}

</ul>

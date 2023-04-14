#pragma once

#define LEN static constexpr int
#define NAME static constexpr const char*

namespace YiZi::Database
{
    namespace User
    {
        NAME name = "User";

        namespace Item
        {
            NAME id = "id";
            NAME phone = "phone";
            NAME password = "password";
            NAME nickname = "nickname";
            NAME join_time = "join_time";
            NAME is_admin = "is_admin";
        }

        namespace ItemLength
        {
            // phone
            LEN PHONE_LENGTH = 11;
            // password
            LEN PASSWORD_MIN_LENGTH = 8;
            LEN PASSWORD_MAX_LENGTH = 20;
            // nickname
            LEN NICKNAME_MIN_LENGTH = 2;
            LEN NICKNAME_MAX_LENGTH = 20;
        }
    }

    /*
    namespace Channel
    {
        NAME name = "Channel";

        namespace Item
        {
            NAME id = "id";
            NAME name = "name";
            NAME join_time = "join_time";
            NAME description = "description";
        }

        namespace ItemLength
        {
            // name
            LEN NAME_MAX_LENGTH = 20;
            // description
            LEN DESCRIPTION_MAX_LENGTH = 500;
        }
    }
    */

    namespace Transcript
    {
        NAME name = "Transcript";

        namespace Item
        {
            //NAME cid = "cid";
            NAME uid = "uid";
            NAME time = "time";
            NAME content = "content";
        }

        namespace ItemLength
        {
            // content
            LEN CONTENT_MAX_LENGTH = 500;
        }
    }

    /*
    namespace Mute
    {
        NAME name = "Mute";

        namespace Item
        {
            NAME id = "id";
            NAME start = "start";
            NAME end = "end";
        }
    }

    namespace Ban
    {
        NAME name = "Ban";

        namespace Item = Mute::Item;
    }
    */
}
#undef LEN
#undef NAME

#pragma once

namespace YiZi::Client
{
    enum DialogBoxCommandID
    {
        CID_FAIL = -1,
        CID_LOGOUT = -2,
        CID_VALIDATE_ADMIN_SUCCESS = -3,

        CID_OK = IDOK,
        CID_CANCEL = IDCANCEL,
        CID_ABORT = IDABORT,
        CID_RETRY = IDRETRY,
        CID_IGNORE = IDIGNORE,
        CID_YES = IDYES,
        CID_NO = IDNO,
    };
}

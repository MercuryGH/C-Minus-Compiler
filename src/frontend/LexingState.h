#pragma once

enum class LexingState
{
    START,

    // abc...
    //  ^^^^^
    IN_ID,

    // 123...
    //  ^^^^^
    IN_LITERAL,

    // /?
    //  ^
    IN_SLASH,

    // // ...
    //    ^^^
    IN_SINGLELINE_COMMENT,

    // /* ...
    //    ^^^
    IN_MULTILINE_COMMENT,

    // ... */
    //      ^
    END_MULTILINE_COMMENT,

    // <?
    //  ^
    IN_LESS,

    // >?
    //  ^
    IN_GREATER,

    // =?
    //  ^
    IN_EQUAL,

    // !?
    //  ^
    IN_NOT,

    // Done
    DONE,
};

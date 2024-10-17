/*
 * message_service.x - RPC interface definition
 */

program MESSAGE_PROG {
    version MESSAGE_VERS {
        string send_message(string) = 1;
    } = 1;
} = 0x31234567;  /* A unique program identifier */


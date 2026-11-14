/*
 * Distilled Ghidra pseudocode for review only. Not OEM source, not buildable,
 * and not safe to run: all addresses are exact-build observations.
 */

struct piu_request {
    unsigned char opcode;
    unsigned char no_reply;
    unsigned short payload_bytes;
    unsigned char payload[];
};

static void piu_isr_model(void)
{
    unsigned status = MMIO_9053204C;
    if (status & 0x08) {
        reply_pending++;
        MMIO_9053204C = 0x08;
    }
    if (status & 0x10) {
        command_pending++;
        MMIO_9053204C = 0x10;
    }
    /* The original ISR returns 2 so that eCos schedules the DSR. */
}

static void piu_dsr_model(void)
{
    if (reply_pending) {
        ecos_flag_set(reply_event, 1);
        reply_pending--;
    }
    if (command_pending) {
        wake_dispatcher(command_wait_object);
        command_pending--;
    }
}

static void dispatcher_model(void)
{
    for (;;) {
        struct piu_request *request = copy_from_ceva_alias(MMIO_90532040);
        void (*handler)(struct piu_request *, unsigned) = callback_table[request->opcode];
        if (handler != 0) {
            handler(request, 4u + request->payload_bytes);
        } else if (!request->no_reply) {
            publish_error_response(request->opcode, 3);
        }
    }
}

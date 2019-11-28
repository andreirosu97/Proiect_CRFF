/*
 * Globals.h
 *
 *  Created on: Nov 14, 2019
 *      Author: Andrei
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_
#include "file_m.h"
#include "file_packet_m.h"

enum MessageType
{
    SEND_FILE_REQUEST,
    SEND_FILE_REQUEST_AKG,
    SEND_FILE_REQUEST_EVENT,
    SEND_FILE_REQUEST_TIME_OUT,
    HOST_FILE_OFFERING,
    RECEIVE_FILE_REQUEST,
    RECEIVE_FILE_REQUEST_EVENT,
    REJECT_OFFER,
    SERVER_START_BC,
    NO_OFFER_RECEIVED_EVENT,
    FILE_PACKAGE,
    FILE_PACKAGE_RECEIVED,
    FILE_PACKAGE_SENT,
    END_TRANSM

};



#endif /* GLOBALS_H_ */

/*
 * Globals.h
 *
 *  Created on: Nov 14, 2019
 *      Author: Andrei
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_
#include "file_m.h"

enum MessageType
{
    SEND_FILE_REQUEST,
    SEND_FILE_REQUEST_AKG,
    SEND_FILE_REQUEST_EVENT,
    SEND_FILE_REQUEST_TIME_OUT,
    HAVE_FILE,
    RECIEVE_FILE_REQUEST,
    RECIEVE_FILE_REQUEST_EVENT

};



#endif /* GLOBALS_H_ */

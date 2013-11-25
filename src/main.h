/*
 * 
 * Copyright (C) 2007 Ken Ellinwood.
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 */
#ifndef __main_h__
#define __main_h__

/* Definitions generated by autotools. */
#if HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#ifdef __cplusplus
extern "C" {
#endif

#include "jpmidi.h"
#include <jack/jack.h>
#include <jack/types.h>
    
/** Show program usage. */
void main_showusage();

/** Show program version. */
void main_showversion(void);

jpmidi_root_t* main_get_jpmidi_root(); ///< The data created by loading and processing a MIDI file.

int main_is_jack_client();///< Returns true if the program connected to jack (-d switch disables jack)
    
#ifdef __cplusplus
}
#endif

#endif /* __main_h__ */
    
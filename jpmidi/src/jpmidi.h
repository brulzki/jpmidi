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

#ifndef __jpmidi_h__
#define __jpmidi_h__

/* Definitions generated by autotools. */
#if HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <glib.h>
#include <jack/jack.h>
#include <jack/midiport.h>

#include "elements.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct jpmidi_channel jpmidi_channel_t;    
typedef struct jpmidi_root jpmidi_root_t;
typedef struct jpmidi_time jpmidi_time_t;
typedef struct jpmidi_event jpmidi_event_t;

struct jpmidi_channel {
    char* program;
    int has_data;
    int number; 
    int muted;
};
    
/** Root data structure containing jpmidi data. */
struct jpmidi_root
{
    char* filename;                 /**< Pathname of the MIDI file. */
    jpmidi_time_t* head;            /**< Head of event list ordered by time. */
    jpmidi_time_t* tail;            /**< Last event list ordered by time. */
    GTree* data;                    /**< jpmidi_time_t* indexed by jack frame, for sorting/searching data by time. */

    struct rootElement* pmidi_root; /**< Data created by the SMF parser. */
    uint16_t time_base;             /**< Time base as specified in the SMF file header as ticks per quarter note. */
    
    jack_nframes_t sample_rate;     /**< Jack sample rate. */
    jack_nframes_t last_frame;      /**< Frame of the last event. */
    jack_nframes_t xtempo_frame;    /**< Jack frame of last tempo change. */
    uint32_t xtempo_tick;           /**< SMF tick at last tempo change. */
    uint32_t tempo_mpq;             /**< Current tempo in microseconds per quarter note. */
    double samples_per_tick;        /**< Current samples/tick value. */

    int send_sysex;                 /**< Set to 0 to disable sending sysex messages. */
    int solo_channel;               /**< When soloing, this is a number between 0 and 15 inclusive. */
    jpmidi_channel_t channel[16];   /**< Channel descriptors. */
};

/** Event data which occurs at a specific time.  This references data on all channels at the given time. */
struct jpmidi_time
{
    jpmidi_time_t* next_time;/**< Pointer to data that is next in time. */
    uint32_t       smf_time; /**< Time as specified in the standard MIDI file. */
    jack_nframes_t frame;    /**< Absolute frame for the associated events. Only usable for internal timing. */
    GArray*        events;   /**< Array of jpmidi_event_t* containing the events for the given time. */
};

/** Data for one event. */
struct jpmidi_event
{
    struct element* element; /**< The pmidi element structure created during SMF parse. */
    GByteArray*     data ;   /**< Raw midi data for this event.  Begins with a MIDI status byte. */
    jpmidi_event_t* related; /**< Experimental, references related note on/off event. */
};
    
/** Initialize this feature.  Must be called before anything else. Returns 1 on success, 0 on failure. */
int jpmidi_init();
    
/** Loads given the midi file. */
jpmidi_root_t* jpmidi_loadfile(char *filename, jack_nframes_t sample_rate);

/** Callback type for listeners interested in file loaded/unloaded event notificaton. */
typedef void (*jpmidi_loadfile_listener_t)(jpmidi_root_t* root);

/** Add a listeners interested in file loaded/unloaded event
 * notificaton. The callback is called with root=NULL if the file is
 * unloaded from memory.
 */    
void jpmidi_add_loadfile_listener( jpmidi_loadfile_listener_t);
    
/** Remove a listener. */    
void jpmidi_remove_loadfile_listener( jpmidi_loadfile_listener_t);
    
/** Create the root data structure for a parsed SMF file. */    
jpmidi_root_t* jpmidi_root_new( char* filename, struct rootElement* proot, jack_nframes_t sample_rate);
    
/** Free the root data structure. */    
void jpmidi_root_free( jpmidi_root_t* root);

/** Returns the pathname of the MIDI file that was loaded and processed. */
char* jpmidi_get_filename( jpmidi_root_t* root);

/** Returns the timebase of the SMF file we loaded. */
uint16_t jpmidi_get_smf_timebase( jpmidi_root_t* root);
    
/** Returns the first time record. */
jpmidi_time_t* jpmidi_get_time_head( jpmidi_root_t* root);
    
/** Ensure that a jpmidi_time_t struct exists for the given time.
 *  This function returns an existing struct, or creates one if it
 *  does not exist yet. */
jpmidi_time_t* jpmidi_get_time( jpmidi_root_t* root, uint32_t smf_time);

/** Solo the specified channel.  Returns 0 on success, 1 otherwise. */
int jpmidi_solo_channel( jpmidi_root_t* root, int channel);
    
/** Mute the specified channel.  Returns 0 on success, 1 otherwise. */
int jpmidi_mute_channel( jpmidi_root_t* root, int channel);
    
/** Unmute the specified channel.  Returns 0 on success, 1 otherwise. */
int jpmidi_unmute_channel( jpmidi_root_t* root, int channel);
    
/** Create/init a new time struct. */
jpmidi_time_t* jpmidi_time_new( uint32_t smf_time, jack_nframes_t frame);

/** Free a time struct. */    
void jpmidi_time_free( jpmidi_time_t* time);

/** Compare function for our GTree which stores time structs order by jack frame time. */    
gint  jpmidi_time_compare(gconstpointer  a, gconstpointer  b);

/** Add an event to a time struct. */    
void jpmidi_time_add_event( jpmidi_time_t* time, jpmidi_event_t* event);

/** Create a new event struct. */
jpmidi_event_t* jpmidi_event_new( struct element* element);

/** Free an event struct. */    
void jpmidi_event_free( jpmidi_event_t* event);

/** Returns a time structure at or within one second before the
 *  specified frame. The return value may be NULL if the given frame is
 *  beyond the end of MIDI playback.
 */
jpmidi_time_t* jpmidi_lookup_entrypoint( jpmidi_root_t* root, jack_nframes_t frame);

/** Returns true if sending of system exclusive messages is enabled. */
int jpmidi_is_send_sysex_enabled( jpmidi_root_t* root);

/** Enable/disable sending of system exclusive messages. */
void jpmidi_set_send_sysex_enabled( jpmidi_root_t* root, int enabled);

/** If a channel is being solo'ed, returns a value between 0 and
 * 15 inclusive.  Returns -1 if no channel is set for solo.
 */
int jpmidi_get_solo_channel( jpmidi_root_t* root);

/** Returns true if the specified channel is currently muted. */
int jpmidi_channel_is_muted( jpmidi_root_t* root, int channel);

/** Returns true if the specified channel has data. */
int jpmidi_channel_has_data( jpmidi_root_t* root, int channel);

/** Returns the human readable channel number for the channel.  This is just channel+1. */
int jpmidi_channel_get_number( jpmidi_root_t* root, int channel);

/** Returns a description based on program change found in the
 * channel.  Based on general MIDI patchset.
 */
char* jpmidi_channel_get_program( jpmidi_root_t* root, int channel);

/** Returns the frame time of the given jpmidi_time object. */
jack_nframes_t jpmidi_time_get_frame( jpmidi_time_t* time);

/** Returns the event tick from the MIDI File (resolution specified in the file's MThd chunk). */
uint32_t jpmidi_time_get_smf_time( jpmidi_time_t* time);

/** Returns the next time object.  Time object are, obviously,
 * order by ascending frame time. The return value is NULL if the
 * end of MIDI data has been reached.
 */
jpmidi_time_t* jpmidi_time_get_next( jpmidi_time_t* time);

/** Returns the number of events at the given time.  The value may
 * be zero if the time object is an entrypoint marker.
 */
int jpmidi_time_get_event_count( jpmidi_time_t* time);

/** Returns the event object at the given index.  The value must
 * be less than the event count value returned by
 * jpmidi_time_get_event_count().
 */
jpmidi_event_t* jpmidi_time_get_event( jpmidi_time_t* time, int index);

/** Returns true if the event represents a system exclusive message. */
int jpmidi_event_is_sysex( jpmidi_event_t*  event);

/** Returns the channel to which the event is assigned. */
int jpmidi_event_get_channel( jpmidi_event_t*  event);

/** Returns the length in bytes of the associated message. */
int jpmidi_event_get_data_length( jpmidi_event_t*  event);

/** Returns the event's MIDI data. */
unsigned char* jpmidi_event_get_data( jpmidi_event_t*  event);

/** Returns the event's status byte with the channel bits cleared. */
unsigned char jpmidi_event_get_status( jpmidi_event_t*  event);
    
#ifdef __cplusplus
}
#endif

#endif /* __jpmidi_h__ */
    

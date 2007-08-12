/*
 *
 *  BlueZ - Bluetooth protocol stack for Linux
 *
 *  Copyright (C) 2004-2007  Marcel Holtmann <marcel@holtmann.org>
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */
#ifndef __AVDTP_H__
#define __AVDTP_H__

#include <stdint.h>
#include <bluetooth/bluetooth.h>

struct avdtp;
struct avdtp_stream;
struct avdtp_local_sep;
struct avdtp_remote_sep;
struct avdtp_error;

/* SEP capability categories */
#define AVDTP_MEDIA_TRANSPORT			0x01
#define AVDTP_REPORTING				0x02
#define AVDTP_RECOVERY				0x03
#define AVDTP_CONTENT_PROTECTION		0x04
#define AVDTP_HEADER_COMPRESSION		0x05
#define AVDTP_MULTIPLEXING			0x06
#define AVDTP_MEDIA_CODEC			0x07

/* AVDTP error definitions */
#define AVDTP_BAD_HEADER_FORMAT			0x01
#define AVDTP_BAD_LENGTH			0x11
#define AVDTP_BAD_ACP_SEID			0x12
#define AVDTP_SEP_IN_USE			0x13
#define AVDTP_SEP_NOT_IN_USE			0x14
#define AVDTP_BAD_SERV_CATEGORY			0x17
#define AVDTP_BAD_PAYLOAD_FORMAT		0x18
#define AVDTP_NOT_SUPPORTED_COMMAND		0x19
#define AVDTP_INVALID_CAPABILITIES		0x1A
#define AVDTP_BAD_RECOVERY_TYPE			0x22
#define AVDTP_BAD_MEDIA_TRANSPORT_FORMAT	0x23
#define AVDTP_BAD_RECOVERY_FORMAT		0x25
#define AVDTP_BAD_ROHC_FORMAT			0x26
#define AVDTP_BAD_CP_FORMAT			0x27
#define AVDTP_BAD_MULTIPLEXING_FORMAT		0x28
#define AVDTP_UNSUPPORTED_CONFIGURATION		0x29
#define AVDTP_BAD_STATE				0x31

/* SEP types definitions */
#define AVDTP_SEP_TYPE_SOURCE			0x00
#define AVDTP_SEP_TYPE_SINK			0x01

/* Media types definitions */
#define AVDTP_MEDIA_TYPE_AUDIO			0x00
#define AVDTP_MEDIA_TYPE_VIDEO			0x01
#define AVDTP_MEDIA_TYPE_MULTIMEDIA		0x02

typedef enum {
	AVDTP_STATE_IDLE,
	AVDTP_STATE_CONFIGURED,
	AVDTP_STATE_OPEN,
	AVDTP_STATE_STREAMING,
	AVDTP_STATE_CLOSING,
	AVDTP_STATE_ABORTING,
} avdtp_state_t;

struct avdtp_service_capability {
	uint8_t category;
	uint8_t length;
	uint8_t data[0];
} __attribute__ ((packed));

struct avdtp_media_codec_capability {
	uint8_t rfa0:4;
	uint8_t media_type:4;
	uint8_t media_codec_type;
	uint8_t data[0];
} __attribute__ ((packed));

typedef void (*avdtp_stream_state_cb) (struct avdtp_stream *stream,
					avdtp_state_t old_state,
					avdtp_state_t new_state,
					struct avdtp_error *err,
					void *user_data);

/* Callbacks for when a reply is received to a command that we sent */
struct avdtp_sep_cfm {
	void (*set_configuration) (struct avdtp_local_sep *lsep,
					struct avdtp_stream *stream);
	void (*get_configuration) (struct avdtp_local_sep *lsep,
					struct avdtp_stream *stream);
	void (*open) (struct avdtp_local_sep *lsep,
				struct avdtp_stream *stream);
	void (*start) (struct avdtp_local_sep *lsep,
			struct avdtp_stream *stream);
	void (*suspend) (struct avdtp_local_sep *lsep,
				struct avdtp_stream *stream);
	void (*close) (struct avdtp_local_sep *lsep,
				struct avdtp_stream *stream);
	void (*abort) (struct avdtp_local_sep *lsep,
				struct avdtp_stream *stream);
	void (*reconfigure) (struct avdtp_local_sep *lsep);
};

/* Callbacks for indicating when we received a new command. The return value
 * indicates whether the command should be rejected or accepted */
struct avdtp_sep_ind {
	gboolean (*get_capability) (struct avdtp_local_sep *sep,
					GSList **caps, uint8_t *err);
	gboolean (*set_configuration) (struct avdtp *session,
					struct avdtp_local_sep *lsep,
					struct avdtp_stream *stream,
					GSList *caps, uint8_t *err,
					uint8_t *category);
	gboolean (*get_configuration) (struct avdtp_local_sep *lsep,
					uint8_t *err);
	gboolean (*open) (struct avdtp_local_sep *lsep,
				struct avdtp_stream *stream, uint8_t *err);
	gboolean (*start) (struct avdtp_local_sep *lsep,
				struct avdtp_stream *stream,
				uint8_t *err);
	gboolean (*suspend) (struct avdtp_local_sep *sep,
				struct avdtp_stream *stream,
				uint8_t *err);
	gboolean (*close) (struct avdtp_local_sep *sep,
				struct avdtp_stream *stream,
				uint8_t *err);
	gboolean (*abort) (struct avdtp_local_sep *sep,
				struct avdtp_stream *stream, uint8_t *err);
	gboolean (*reconfigure) (struct avdtp_local_sep *lsep, uint8_t *err);
};

typedef void (*avdtp_discover_cb_t) (struct avdtp *session, GSList *seps,
					int err, void *user_data);

struct avdtp *avdtp_get(bdaddr_t *src, bdaddr_t *dst);

void avdtp_unref(struct avdtp *session);
struct avdtp *avdtp_ref(struct avdtp *session);

struct avdtp_service_capability *avdtp_service_cap_new(uint8_t category,
							void *data, int size);

struct avdtp_service_capability *avdtp_get_codec(struct avdtp_remote_sep *sep);

int avdtp_discover(struct avdtp *session, avdtp_discover_cb_t cb,
			void *user_data);

void avdtp_stream_set_cb(struct avdtp *session, struct avdtp_stream *stream,
				avdtp_stream_state_cb cb, void *data);

gboolean avdtp_stream_get_transport(struct avdtp_stream *stream, int *sock,
					uint16_t *mtu, GSList **caps);

int avdtp_set_configuration(struct avdtp *session,
				struct avdtp_remote_sep *rsep,
				struct avdtp_local_sep *lsep,
				GSList *caps,
				struct avdtp_stream **stream);

int avdtp_get_configuration(struct avdtp *session,
				struct avdtp_stream *stream);

int avdtp_open(struct avdtp *session, struct avdtp_stream *stream);
int avdtp_reconfigure(struct avdtp *session, struct avdtp_stream *stream);
int avdtp_start(struct avdtp *session, struct avdtp_stream *stream);
int avdtp_suspend(struct avdtp *session, struct avdtp_stream *stream);
int avdtp_close(struct avdtp *session, struct avdtp_stream *stream);
int avdtp_abort(struct avdtp *session, struct avdtp_stream *stream);

struct avdtp_local_sep *avdtp_register_sep(uint8_t type, uint8_t media_type,
						struct avdtp_sep_ind *ind,
						struct avdtp_sep_cfm *cfm);

/* Find a matching pair of local and remote SEP ID's */
int avdtp_get_seps(struct avdtp *session, uint8_t type, uint8_t media,
			uint8_t codec, struct avdtp_local_sep **lsep,
			struct avdtp_remote_sep **rsep);

int avdtp_unregister_sep(struct avdtp_local_sep *sep);

const char *avdtp_strerror(struct avdtp_error *err);

void avdtp_get_peers(struct avdtp *session, bdaddr_t *src, bdaddr_t *dst);

int avdtp_init(void);
void avdtp_exit(void);

#endif

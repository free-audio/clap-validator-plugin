/*
 * Clap Validator Plugin
 *
 * A expanding set of plugins to test your host
 *
 * Copyright 2024-2025,Various authors, as described in the github
 * transaction log.
 *
 * This source repo is released under the MIT license,
 * The source code and license are at https://github.com/free-audio/clap-validator-plugin
 */

#ifndef FREEAUDIO_CVP_CVP_ENTRY_IMPL_H
#define FREEAUDIO_CVP_CVP_ENTRY_IMPL_H

namespace free_audio::cvp
{
const void *get_factory(const char *factory_id);
bool clap_init(const char *p);
void clap_deinit();
} // namespace free_audio::cvp

#endif
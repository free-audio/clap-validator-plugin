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

#include "cvp-entry-impl.h"
#include "cvp-plugin.h"

#include <cstring>
#include <cassert>

namespace free_audio::cvp
{
namespace detail
{
uint32_t clap_get_plugin_count(const clap_plugin_factory *) { return pluginCount; };
const clap_plugin_descriptor *clap_get_plugin_descriptor(const clap_plugin_factory *f, uint32_t idx)
{
    assert(idx < clap_get_plugin_count(f));
    auto vf = (ValidatorFlavor)idx;
    return getDescriptor(vf);
}

const clap_plugin_t *clap_create_plugin(const clap_plugin_factory *f, const clap_host_t *host,
                                        const char *desc)
{
    for (int i = 0; i < clap_get_plugin_count(f); ++i)
    {
        if (strcmp(desc, getDescriptor((ValidatorFlavor)i)->id) == 0)
        {
            return makePlugin(host, (ValidatorFlavor)i);
        }
    }
    return nullptr;
}
} // namespace detail
bool clap_init(const char *p) { return true; }
void clap_deinit() {}

const void *get_factory(const char *factory_id)
{
    if (strcmp(factory_id, CLAP_PLUGIN_FACTORY_ID) == 0)
    {
        static const struct clap_plugin_factory six_sines_clap_factory = {
            detail::clap_get_plugin_count,
            detail::clap_get_plugin_descriptor,
            detail::clap_create_plugin,
        };
        return &six_sines_clap_factory;
    }
    return nullptr;
}
} // namespace free_audio::cvp
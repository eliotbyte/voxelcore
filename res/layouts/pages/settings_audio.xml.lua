function create_setting(id, name, step, postfix)
    local info = core.get_setting_info(id)
    postfix = postfix or ""
    document.root:add(gui.template("track_setting", {
        id=id,
        name=gui.str(name, "settings"),
        value=core.get_setting(id),
        min=info.min,
        max=info.max,
        step=step,
        postfix=postfix
    }))
    update_setting(core.get_setting(id), id, name, postfix)
end

function update_setting(x, id, name, postfix)
    core.set_setting(id, x)
    -- updating label
    document[id..".L"].text = string.format(
        "%s: %s%s", 
        gui.str(name, "settings"), 
        core.str_setting(id), 
        postfix
    )
end

local initialized = false

function on_open()
    if not initialized then
        initialized = true
        local token = audio.input.__get_core_token()
        document.root:add("<container id='tm' />")
        local prev_amplitude = 0.0
        document.tm:setInterval(16, function()
            audio.input.fetch_input(token)
            local amplitude = audio.input.get_max_amplitude()
            if amplitude > 0.0 then
                amplitude = math.sqrt(amplitude)
            end
            document.input_volume_inner.size = {
                prev_amplitude *
                document.input_volume_outer.size[1],
                document.input_volume_outer.size[2]
            }
            prev_amplitude = amplitude * 0.25 + prev_amplitude * 0.75
        end)
    end
    create_setting("audio.volume-master", "Master Volume", 0.01)
    create_setting("audio.volume-regular", "Regular Sounds", 0.01)
    create_setting("audio.volume-ui", "UI Sounds", 0.01)
    create_setting("audio.volume-ambient", "Ambient", 0.01)
    create_setting("audio.volume-music", "Music", 0.01)
    document.root:add("<select id='input_device_select' "..
        "onselect='function(opt) audio.set_input_device(opt) end'/>")
    document.root:add("<container id='input_volume_outer' color='#000000' size='4'>"
                        .."<container id='input_volume_inner' color='#00FF00FF' size='4'/>"
                    .."</container>")
    local selectbox = document.input_device_select
    local devices = {}
    local names = audio.get_input_devices_names()
    for i, name in ipairs(names) do
        table.insert(devices, {value=name, text=name})
    end
    selectbox.options = devices
    selectbox.value = audio.get_input_info().device_specifier
end

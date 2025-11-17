local updating_blocks = {}
local TYPE_REGISTER = 0
local TYPE_UNREGISTER = 1

block.__perform_ticks = function(delta)
    for id, entry in pairs(updating_blocks) do
        entry.timer = entry.timer + delta
        local steps = math.floor(entry.timer / entry.delta * #entry / 3)
        if steps == 0 then
            goto continue
        end
        entry.timer = 0.0
        local event = entry.event
        local tps = entry.tps
        for i=1, steps do
            local x = entry[entry.pointer + 1]
            local y = entry[entry.pointer + 2]
            local z = entry[entry.pointer + 3]
            entry.pointer = (entry.pointer + 3) % #entry
            events.emit(event, x, y, z, tps)
        end
        ::continue::
    end
end

block.__process_register_events = function()
    local register_events = block.__pull_register_events()
    if not register_events then
        return
    end
    for i=1, #register_events, 4 do
        local header = register_events[i]
        local event_bits = bit.band(header, 0xFFFF)
        local id = bit.rshift(header, 16)
        local x = register_events[i + 1]
        local y = register_events[i + 2]
        local z = register_events[i + 3]

        local list = updating_blocks[id]
        if bit.band(event_bits, TYPE_REGISTER) ~= 0 then
            if not list then
                list = {}
                list.event = block.name(id) .. ".blocktick"
                list.tps = 20 / (block.properties[id]["tick-interval"] or 1)
                list.delta = 1.0 / list.tps
                list.timer = 0.0
                list.pointer = 0
                updating_blocks[id] = list
            end
            table.insert(list, x)
            table.insert(list, y)
            table.insert(list, z)
        elseif bit.band(event_bits, TYPE_UNREGISTER) ~= 0 then
            if list then
                for j=1, #list, 3 do
                    if list[j] == x and list[j + 1] == y and list[j + 2] == z then
                        for k=1,3 do
                            table.remove(list, j)
                        end
                        j = j - 3
                    end
                end
            end
        end
    end
end

local audio_input_tokens_store = {}
audio.input = {}

local _gui_confirm = gui.confirm
local _base64_encode_urlsafe = base64.encode_urlsafe
local _random_bytes = random.bytes
local _debug_pack_by_frame = debug.get_pack_by_frame
local _audio_fetch_input = audio.__fetch_input
audio.__fetch_input = nil
local MAX_FETCH = 44100 * 4

local total_fetch = Bytearray()

function audio.__reset_fetch_buffer()
    total_fetch:clear()
end

function audio.fetch_input(token, size)
    size = size or MAX_FETCH
    if audio_input_tokens_store[token] then
        if #total_fetch >= size then
            return total_fetch:sub(1, size)
        end
        total_fetch:append(_audio_fetch_input(size - #total_fetch))
        return total_fetch:sub()
    end
    error("access denied")
end

local GRANT_PERMISSION_MSG = "Grant '%{0}' pack audio recording permission?"

function audio.input.request_open(callback)
    local token = _base64_encode_urlsafe(_random_bytes(18))
    local caller = _debug_pack_by_frame(1)
    _gui_confirm(gui.str(GRANT_PERMISSION_MSG):gsub("%%{0}", caller), function()
        audio_input_tokens_store[token] = caller
        callback(token)
        menu:reset()
    end)
end

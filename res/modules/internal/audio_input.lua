local audio_input_tokens_store = {}
audio.input = {}

local _gui_confirm = gui.confirm
local _base64_encode_urlsafe = base64.encode_urlsafe
local _random_bytes = random.bytes
local _debug_pack_by_frame = debug.get_pack_by_frame
local _audio_fetch_input = audio.fetch_input

function audio.fetch_input(token, size)
    if audio_input_tokens_store[token] then
        return _audio_fetch_input(size)
    end
    error("access denied")
end

local GRAND_PERMISSION_MSG = "Grant '%{0}' pack audio recording permission?"

function audio.input.request_open(callback)
    local token = _base64_encode_urlsafe(_random_bytes(18))
    local caller = _debug_pack_by_frame(1)
    _gui_confirm(gui.str(GRAND_PERMISSION_MSG):gsub("%%{0}", caller), function()
        audio_input_tokens_store[token] = caller
        callback(token)
        menu:reset()
    end)
end

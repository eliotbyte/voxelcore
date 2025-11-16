app.create_memory_device("memtest")

debug.log("check initial state")
assert(file.exists("memtest:"))
assert(file.is_writeable("memtest:"))

debug.log("write text file")
assert(file.write("memtest:text.txt", "example, пример"))
assert(file.exists("memtest:text.txt"))

debug.log("read text file")
assert(file.read("memtest:text.txt") == "example, пример")

debug.log("delete file")
file.remove("memtest:text.txt")
assert(not file.exists("memtest:text.txt"))

debug.log("create directory")
file.mkdir("memtest:dir")
assert(file.isdir("memtest:dir"))

debug.log("remove directory")
file.remove("memtest:dir")
assert(not file.isdir("memtest:dir"))

debug.log("create directories")
file.mkdirs("memtest:dir/subdir/other")
assert(file.isdir("memtest:dir/subdir/other"))

debug.log("remove tree")
file.remove_tree("memtest:dir")
assert(not file.isdir("memtest:dir"))

debug.log("write binary file")
local bytes = {0xDE, 0xAD, 0xC0, 0xDE}
file.write_bytes("memtest:binary", bytes)
assert(file.exists("memtest:binary"))

debug.log("write binary file")
local bytes = {0xDE, 0xAD, 0xC0, 0xDE}
file.write_bytes("memtest:binary", bytes)
assert(file.exists("memtest:binary"))

debug.log("read binary file")
local rbytes = file.read_bytes("memtest:binary")
assert(#rbytes == #bytes)
for i, b in ipairs(bytes) do
    assert(rbytes[i] == b)
end

debug.log("delete file")
file.remove("memtest:binary")
assert(not file.exists("memtest:binary"))

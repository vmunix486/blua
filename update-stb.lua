#!/usr/bin/env lua

local downloader = "wget"

if downloader == "wget" then
	downloadcmd = downloader
elseif downloader == "curl" then
	downloadcmd = downloader .. " -O"
else
	print("error")
	os.exit(1)
end

print("Downloader is " .. downloader)

local inih = io.open("src/ini.h", "r")
if nobh then os.remove("src/ini.h") end
print("removed ini.h")

local inic = io.open("src/ini.c", "r")
if inic then os.remove("src/ini.c") end
print("removed ini.c")

print("Downloading ini.h")
os.execute("cd src/ && " ..  downloadcmd .. " https://raw.githubusercontent.com/benhoyt/inih/refs/heads/master/ini.h >/dev/null 2>&1")
print("Done downloading ini.h")

print("Downloading ini.c")
os.execute("cd src/ && " .. downloadcmd .. " https://raw.githubusercontent.com/benhoyt/inih/refs/heads/master/ini.c >/dev/null 2>&1")
print("Done downloading ini.c")

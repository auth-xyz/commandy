---------------------------------------------------------------

So *another* tool with a .txt README...

Anyways, commandy (as I call it), is a simple CLI that
interacts with command-not-found.com, shows you in a pretty way
the command required to install a specific tool.

It has two requirements.

NerdFonts,
and cURL. (curl-devel)

That's it! 
Usage is pretty simple, it doesn't have a lot of things other than
the following flags/arguments

--list (-la)
--command (-c) 
--distro (-d) (not needed if you're using --list)

Usage is, as follows:

./commandy -d fedora -c nmcli
(searching for nmcli, specifically for the command for fedora)

I may implement some sort of installation logic to prompt the user to
install the packages that were queried, but i'm too lazy at the moment!

So enjoy! :)))

---------------------------------------------------------------

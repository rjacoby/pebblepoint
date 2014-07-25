PebblePoint
==================
PebblePoint is a watchapp for [Pebble](http://getpebble.com) smartwatches that lets you control PowerPoint on Mac OS X.

Requirements
------------
- Mac OS X (this server uses AppleScript to control your presentation)
- Microsoft PowerPoint
- A Pebble watch, with [PebblePoint](https://apps.getpebble.com/applications/53cec3433967e94f5500000b) installed
- [PebblePointServer.app](https://db.tt/nvHQZmPg)

Installation
------------
Install the app from the Pebble Store on your phone.

Running the Server
------------------
Download and run [PebblePointServer.app](https://db.tt/nvHQZmPg).

Or, if you're more technical you can install the Node.js package, [pebblepoint_server](https://github.com/rjacoby/pebblepoint_server). It's a lot smaller than the .app version, which has to bundle all of Node.js, Chromium, etc.

Configuring your Pebble
-----------------------
When the server runs, it shows you what IP and PORT you need to use. Follow the on-screen instructions to get going!

Burning Questions
------------------
### Why Mac OS X only?
I initially built this for myself, to use at Razorfish's Tech Summit. I work on a Mac. I build using open source software, which this is (except for the MS part). I don't develop on PCs and I know that building a simple, easily installable REST server to talk to PPT for Windows via VB isn't my bag of tea.

### Well then, Mac Guy: Why PowerPoint and not Keynote?
Simply because our corporate decks are all PPT and that's what I had a need for. It also has a very rich AppleScript command dictionary, which let me bridge the last step into controlling a .app.

### Will you do a PC version?
No. Will you? Please do!

I think the optimal way would be for you to fork the [pebblepoint_server](https://github.com/rjacoby/pebblepoint_server) project and make it smart about OS integration. You'd also need to make the build of an atom-shell .exe for Windows happen.

However, running Node.js on Windows seems tough to me.

So, another option would be to just make a workalike server that speaks the same simple REST protocol. You'd just need to implement something that has the functionality of the `POST /go/:command` section of the [server guts](https://github.com/rjacoby/pebblepoint_server/blob/master/routes/index.coffee) and have a way of showing the IP and PORT to the user for config purposes.

### Your C is ugly.
I agree, but that's not a question.

### What technologies are involved in this project?
Quite a lot, actually.

- C code for the watchapp
- JavaScript on the phone to let the Pebble make network calls to the server
- [JQuery/mobile](http://jquerymobile.com) for form validation in the config screen on the phone
- [Node.js](http://nodejs.org) for building the server
- [Express](http://expressjs.com) to give the server structure and some nice helpers
- [Coffeescript](http://coffeescript.org) because I prefer it to JS
- [Applescript](https://developer.apple.com/library/mac/documentation/AppleScript/Conceptual/AppleScriptLangGuide/introduction/ASLR_intro.html) to control PowerPoint
- [Bootstrap](http://getbootstrap.com) for simple prettying of webviews
- [EJS](http://embeddedjs.com) because even tho I prefer [Jade](http://jade-lang.com), I wanted the server to be a lightweight NPM package and have fewer dependencies, but then I used...
- [atom-shell](https://github.com/atom/atom-shell) to make the jumbo heavyweight (100+MB!!!) standalone app for people who aren't going to do Node package installs and command line stuff
- [Katon](https://github.com/typicode/katon) for running the server as I was developing the package
- [Grunt](http://gruntjs.com) for building, because it had a task for grabbing atom-shell
- [ditto](https://developer.apple.com/library/mac/documentation/Darwin/Reference/ManPages/man1/ditto.1.html) because I gave up on fighting with Node's glob behavior to allow me to copy the atom-shell and all its space-filled filenames and symlinks
- [Markdown](http://daringfireball.net/projects/markdown/) for this file you're reading right now

### Will you do this for other smartwatches?
If you give me a free one, I'll give it a shot.

I was a Kickstarter backer of the Pebble, so that's why I have it.

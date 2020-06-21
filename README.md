# ASI Loader for Mass Effect: Andromeda

**Last game version tested:** 1.09 (build 739369)

Get the latest compiled binary here:<br />
[![Dropbox](./download.png)](https://www.dropbox.com/sh/lvo3oj0lzccoakk/AABVmZI0hzZxeIHfniBJ6V6na?dl=0)

-----------------------------------------------------------

About ASI loader - original code by listener ([XLiveLess](http://gtaforums.com/topic/388658-relgtaiv-xliveless/))<br />
The ASI loader allows for loading of custom-made DLL files without the need of a separate injector.<br />
Such DLL files must be renamed to *.asi and put in a subfolder named *ASI* inside the game's root folder.<br />
For testing purposes, the ASI loader will, alternatively, load plugins from the root if the *ASI* subfolder is missing or nothing was successfully loaded from there.

**You may visit https://github.com/Erik-JS/MEA-ASI for plugins created by me.**

-----------------------------------------------------------

This proxy DLL also patches a call to BCryptVerifySignature from Bcrypt.dll to facilitate file modding.<br />
[Source: Rick from ZenHAX](https://zenhax.com/viewtopic.php?f=9&t=3971&start=140#p22179)<br />
Honorable mention: [Warranty Voider's MEA Explorer](https://github.com/zeroKilo/MEAExplorerWV)

-----------------------------------------------------------

**BUILT WITH [Code::Blocks](http://www.codeblocks.org/) AND [TDM-GCC](https://jmeubank.github.io/tdm-gcc/).**

**DESIGNED FOR WINDOWS 7 OR LATER.**

-----------------------------------------------------------

**Usage:**<br />
- Rename the original DLL file to .bak, then put the new DLL in its place.<br />
- A file called MEA_Ansel64.log will be created in the current user's Documents folder.

-----------------------------------------------------------

This modification is not made or supported by EA or BioWare.<br />
Mass Effect is a trademark of EA International (Studio and Publishing) Ltd.
# Welcome to My Tar
***

## Task
my_tar is a command to manipulate tape archive. The first option to tar is a mode indicator from the following list:

-c Create a new archive containing the specified items.
-r Like -c, but new entries are appended to the archive. The -f option is required.
-t List archive contents to stdout.
-u Like -r, but new entries are added only if they have a modification date newer than the corresponding entry in the archive. The -f option is required.
-x Extract to disk from the archive. If a file with the same name appears more than once in the archive, each copy will be extracted, with later copies overwriting (replacing) earlier copies.
In -c, -r, or -u mode, each specified file or directory is added to the archive in the order specified on the command line. By default, the contents of each directory are also archived.

Unless specifically stated otherwise, options are applicable in all operating modes:

-f file Read the archive from or write the archive to the specified file. The filename can be standard input or standard output.
my_tar will not handle file inside subdirectory.

## Description
In this program I define 4 structs: my_tar_header, s_tar_node, flag_s and arg_s. 
The my_tar_header struct is modeled after the POSIX header struct, see https://www.gnu.org/software/tar/manual/html_node/Standard.html#Standard. 
The flag_s and arg_s structs store parsed options and arguments, respectively. 
The s_tar_node struct is a node which is only used with the -u flag. 
The link list created with s_tar_node stores file name and mtime entried for each archived file in the tarball, in order to compare with new entries.

All function prototypes are included in header.h and organized under the corresponding .c file for easier navigation.

## Installation
TO install the project run "make my_tar" in your command line.

## Usage
List:     my_tar -tf 
Extract:  my_tar -xf 
Create:   my_tar -cf <archive-filename> [filenames...]
Append:   my_tar -rf <archive-filename> [filenames...]
Update:   my_tar -uf <archive-filename> [filenames...]

### The Core Team

<span><i>Made at <a href='https://qwasar.io'>Qwasar SV -- Software Engineering School</a></i></span>

#!/usr/bin/perl

$argind=0;

if ($ARGV[$argind] eq "--product") {
    $product = $ARGV[$argind + 1];
    $argind += 2;
}

#$VER_PATH = "/pub/products/7700aco2-hd/ver/";

if ($ARGV[$argind] eq "--path") {
    $VER_PATH = $ARGV[$argind + 1];
    $argind += 2;
}

$timestring = localtime();
printf "Current time is: $timestring.\n";


if ($product ne "") {
    $VERSION_FILE = $VER_PATH . $product . "-ver.txt";
} else {
    $VERSION_FILE= $VER_PATH . 'version.txt';
}

if (open(VER,$VERSION_FILE)) {
    $_ = <VER>;
    ($major, $minor, $build)=split(/ /, $_, 3);
    close(VER);
} else {
    print "Can not open version file, reset.\n";
    $major=1;
    $minor=0;
    $build=0;
}

if ($ARGV[$argind] eq "--major") {
    print "Major version number change.\n";
    $major++;
    $minor=0;
    $build=1;
    $argind++;
} elsif ($ARGV[$argind] eq "--minor") {
    print "Minor version number change.\n";
    $minor++;
    $build=1;
    $argind++;
} else {
    $build++;
}

open(VER,">$VERSION_FILE") || die "Couldn't write version data.\n";
print VER "$major $minor $build";
close(VER);

open(H,'>ver.h') || die "Couldn't write ver.h\n";
print H "#ifndef VER_H\n";
print H "#define VER_H\n";
print H "#define SW_VERSION_MAJOR $major\n";
print H "#define MAJOR_STR \"$major\"\n";
print H "#define SW_VERSION_MINOR $minor\n";
print H "#define MINOR_STR \"$minor\"\n";
print H "#define BUILD $build\n";
print H "#define BUILD_STR \"$build\"\n";
print H "#define BUILD_TIME_STR \"$timestring\"\n";
print H "#define CARD_NAME_STR \"$product\"\n";
print H "extern char ver[];\n";
print H "extern char ver_build[];\n";
print H "extern char build_time[];\n";
print H "extern char card_name[];\n";
print H "extern char build_string[];\n";
print H "extern int sw_revision_major;\n";
print H "extern int sw_revision_minor;\n";
print H "#endif\n\n\n";
close H;

open(C,'>ver.c') || die "Couldn't write ver.c.\n";
print C "#include \"ver.h\"\n";
print C "char ver[] = MAJOR_STR \".\" MINOR_STR;\n";
print C "char ver_build[] = MAJOR_STR \".\" MINOR_STR \" build \" BUILD_STR;\n";
print C "char build_time[] = BUILD_TIME_STR;\n";
print C "char build_string[] = BUILD_STR;\n";
print C "int sw_revision_major = SW_VERSION_MAJOR;\n";
print C "int sw_revision_minor = SW_VERSION_MINOR;\n\n\n";

close C;

open(V,'>version') || die "Couldn't write version\n";
print V "$product version $major.$minor.$build\n";
close V;

print "version $major.$minor build $build.\n";


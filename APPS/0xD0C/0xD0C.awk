#
# 0xD0C.awk -- extract LaTeX-style documentation from C header files
#






# TODO -- brug listings, C style til formatering af prototyper og datatyper
#      -- \label{function-foo}




function product_end () {
    print "\\null\\thispagestyle{empty}\\null\\newpage"   > document
    print ""   > document
    print "\\end{document}"   > document
    close(document)
}



function product_begin () {
    if (PRODUCT_STARTED)
        product_end()

    PRODUCT_STARTED = 1
    PRODUCTNAME = $4
    document  = PRODUCTNAME ".tex"
    colophon  = PRODUCTNAME "colophon.tex"
    preamble  = PRODUCTNAME "pre.tex"

    print "pdflatex " PRODUCTNAME >"build.cmd"
    print "pdflatex " PRODUCTNAME >"build.cmd"

    print "\\product{" PRODUCTNAME "}" > preamble
    print "\\renewcommand{\\reportnumber}{" PRODUCTNAME "}" > preamble

    DESCRIPTION = 0
    BLOCKQUOTE = 0
    VERBATIM = 0
    in_group = 0
    ESCAPE_UNDERSCORES = 1;



    print "% colophon text goes here" > colophon

    print "\\documentclass[final,12pt,twoside,onecolumn,openany,a4paper]{book}"     > document
    print "\\providecommand{\\reportnumber}{1}"                                     > document
    print "\\providecommand{\\ISBNidentification}{978-78-92107-42-8}"               > document
#    print "\\providecommand{\\imagefile}{forside.jpg}"
    print "\\providecommand{\\imagefile}{}"                 > document
    print "\\usepackage{0xD0C}"                             > document


    print "\\input{" PRODUCTNAME "pre}"                                    > document
    print "\\title{\\theproductname\\ -- \\thesummary}"      > document


    #--------------------------------------------------------------------
    # this creates the header and removes pagination for the first page
    #--------------------------------------------------------------------
    print "\\begin{document}\\onecolumn\\selectlanguage{english}" > document
    print ""                                                      > document
    print "\\null\\vfill"                                         > document

    # COLOPHON
    print "\\colophonstart" > document
    print "" > document
    print "\\vspace{2em}" > document
    print "\\noindent" > document
    print "\\input{" PRODUCTNAME "colophon}" > document
    print "\\par\\noindent" > document
    print "" > document
    print "\\tableofcontents" > document

    print "\\setlength{\\parskip}{2ex plus 0.5ex minus 0.2ex}" > document
    print "\\setlength{\\parindent}{0pt}" > document

    print "\\chapter[\\theproductname\\  -- \\thesummary]{\\theproductname}" > document
    print "\\emph{\\large\\null\\hfill \\thesummary}" > document
}







BEGIN {
    DESCRIPTION = 0
    BLOCKQUOTE = 0
    VERBATIM = 0
    PRODUCTNAME = ""
    PRODUCT_STARTED = 0
    ESCAPE_UNDERSCORES = 1;
    in_group = 0
    print "" >> "build.cmd"
    document  = "ERROR.out.tex"
    colophon  = "ERROR.out.colophon.tex"
    preamble  = "ERROR.out.pre.tex"
}


END {
    product_end()
}



#
# Strip leading and trailing whitespace
#
function stripws () {
    gsub (/^[[:space:]|[:cntrl:]|\x20|\r|\n]+|[[:space:]|[:cntrl:]|\r|\n]+$/, "",$0)
}

#
# Strip off $1 and leading and trailing whitespace
#
function strip (n) {
    while (n--) {
        $1 = ""
        stripws()
    }
}

#
# Strip off $NF and leading and trailing whitespace
#
function pirts (n) {
    while (n--) {
        $(NF) = ""
        stripws()
    }
}



function start_group () {
    if ($4 == "Continued")
        return
    if (in_group) {
        print "\\bigskip\\noindent\\disappearingrule\\bigskip\n\n"  > document
        return
    }
    in_group = 1
}


# ----------------------------------------------------------------
# Product
# ----------------------------------------------------------------
# A comment of the form /* -- Product -------------- */ indicates
# the start of the descriptive content of the header file.
# This will provoke the introduction of the introductory section
# of the documentation, with an appropriate headline etc.
# ----------------------------------------------------------------

/\/\* -- PRODUCT / {
    product_begin()
}




# ----------------------------------------------------------------
# Module
# ----------------------------------------------------------------
# A comment of the form /* -- Module -------------- */ indicates
# the start of a block of related functionality. In LaTeX terms,
# a module introduces another "Part" of documentation.
# ----------------------------------------------------------------

/\/\* -- MODULE / {
    strip(3)
    pirts(2)

    PARTNAME = $0
    print "\\part{" PARTNAME "}" > document
    in_group = 0
}


# ----------------------------------------------------------------
# Group
# ----------------------------------------------------------------
# A comment of the form /* -- Group ... -- */ indicates
# the start of a new group of functionality within a module.
# This will provoke the introduction of a new chapter-level
# headline and table-of-contents entry.
# ----------------------------------------------------------------

/\/\* -- GROUP / {
    strip(3)
    pirts(2)
        
    GROUPNAME = $0
    print "\\chapter{" GROUPNAME "}" > document
    in_group = 0
}


# ----------------------------------------------------------------
# Function
# ----------------------------------------------------------------
# A comment of the form /* -- Function -------------- */ indicates
# that the next line is a function prototype, which should
# provoke the introduction of a new documentation section with
# appropriate headline etc.
# ----------------------------------------------------------------

/\/\* -- Function / {
    start_group()

    getline
    # gsub ("_", "\\_")     # escape underscores
    stripws()

    # the prototype is the entire line (and possibly more)
    function_prototype = $0

    # the function name is the token immediately before the
    # first token starting with an opening parenthesis "("
    # TODO: support style without spacing between
    #       function name and opening parenthesis
    for (i = 1; i < NF; i++) {
        if ($(i+1) ~ /^\(/) {
            function_name = $(i)
            gsub ("_", "\\_", function_name)     # escape underscores
            break
        }
    }
    
    # read and add more lines until we reach one with the
    # ");" prototype terminator
    while ($0 !~ /);/) {
        getline
        function_prototype = function_prototype "\n" $0
    }
    printf ("\\function{%s}\n", function_name)      > document
    printf ("\\prototype{%s}\n\n\n", function_prototype) > document
    next
}




# ----------------------------------------------------------------
# Datatype
# ----------------------------------------------------------------
# A comment of the form /* -- Datatype -------------- */ indicates
# that the next line is a data type definition, which should
# provoke the introduction of a new documentation section with
# appropriate headline etc.
#
# TODO: Support multi line definitions (read until semicolon)
#       Format using listings environment
#
# ----------------------------------------------------------------

/\/\* -- Datatype / {
    start_group()

    getline
    # gsub ("_", "\\_")     # escape underscores
    gsub (/\r/,"");       # remove carriage returns
    gsub (/\n/,"");       # remove newlines

    # the data type definition is the entire line (and possibly more)
    data_type_definition = $0
    
    # what kind of data type are we eating?
    data_type_type = $1
    terminator = ";"
    if (index ("struct union enum", data_type_type)) {
        data_type_name = $1 " " $2
        gsub ("{", "",    data_type_name);
        terminator = "};"
    }

    # read and add more lines until we reach one with the
    # ";" data type terminator
    while (! match ($(NF), terminator)) {
        getline
        data_type_definition = data_type_definition "\n" $0
    }

    if (terminator == ";") {
        # the data type name is the last token of the definition,
        # with the trailing semicolon stripped off
        data_type_name = $(NF)
        gsub (";", "", data_type_name);
    }
    
    gsub ("_", "\\_", data_type_name)     # escape underscores
    printf ("\\function{%s}\n", data_type_name) > document
    print  "\\lstset{style=datatype}%"          > document
    print  "\\begin{lstlisting}"                > document
    print  data_type_definition                 > document
    print  "\\end{lstlisting}"                  > document

    # printf ("\\datatype{%s}\n", data_type_definition)
    next
}



# ----------------------------------------------------------------
# Macro
# ----------------------------------------------------------------
# A comment of the form /* -- Macro ------------- */ indicates
# that the next line is a macro
# ----------------------------------------------------------------

/\/\* -- Macro / {
    start_group()

    getline
    # gsub ("_", "\\_")     # escape underscores
    stripws()

    # the definition is the entire line (and possibly more)
    macro_definition = $0

    # the macro name is the second token
    # TODO: support "#   define   pap" style
    macro_name = $2
    gsub ("_", "\\_", macro_name)     # escape underscores
    
    # read and add more lines until we reach one without
    # the macro continuation "\" end-of-line escape.
    while ($0 ~ /\$/) {
        getline
        macro_definition = macro_definition "\n" $0
    }

    print  "\\function{" macro_name "}"         > document
    print  "\\lstset{style=datatype}%"          > document
    print  "\\begin{lstlisting}"                > document
    print     macro_definition                  > document
    print  "\\end{lstlisting}"                  > document

    next
}



function blockquote () {
    strip(1)
    if (!BLOCKQUOTE)
        print "\\begin{quote}"                  > document
    BLOCKQUOTE = 1
    print                 > document
    next
}

function verbatim () {
    strip(1)
    if (!VERBATIM)
        print "\\begin{lstlisting}[breaklines,basicstyle=\\ttfamily]"  > document
    VERBATIM = 1
    print                                           > document
    next
}

function description () {
    strip(1)
    if ($1=="END") {
        print "\\end{description}"                    > document
        DESCRIPTION = 0
        next
    }
    if (!DESCRIPTION)
        print "\\begin{description}"                  > document
    DESCRIPTION = 1
    # print "\\item[" $1 "] \\hfill\\\\"                > document
    print "\\item[" $1 "] "                > document
    strip(1)
    print                                             > document
    next
}


function endblocks () {
    if (BLOCKQUOTE && ($1!="*")) {
        print "\\end{quote}"                          > document
        BLOCKQUOTE = 0
        next
    }

    if (VERBATIM && ($1!=".")) {
        print "\\end{lstlisting}"                    > document
        VERBATIM = 0
        print "% ending verbatim"                     > document
        next
    }

}


/\\begin{lstlisting}/ {ESCAPE_UNDERSCORES = 0}
/\\end{lstlisting}/   {ESCAPE_UNDERSCORES = 1}


# ----------------------------------------------------------------
# 0xD0C
# ----------------------------------------------------------------
# Everything between an #if !0xD0C and #endif is regarded as
# plain LaTeX-style 0xD0C-documentation
#
# Addtional syntactic sugar:
# ----------------------------------------------------------------

/0xD0C/,/^#endif/ {

    #
    # Precondition input:
    #
    # Remove leading and trailing whitespace, especially in order to
    #     avoid having Windows style line breaks messing things up
    #     with appended '\r' characters.
    # Ignore lines with leading hashmarks (means we're at the start
    #     or end of "#if !0xD0C")
    # Escape all underscores (they signify subscripts in LaTeX)
    #
    stripws()
    if ($1 ~ "^#")
        next
    if ((ESCAPE_UNDERSCORES==1) && ($1!="."))
        gsub("_", "\\_")
    # XXX should we also escape "#&\" ?

    #
    # Store title page information in the file "pre.tex"
    #
    if (/\\author/ || /\\title/ || /\\date/ || /\summary/) {
        print > PRODUCTNAME "pre.tex"
        next
    }


    #
    # Handle ".FILE filename message..." syntax for writing additional
    # info to any named file (this feature is primarily intended
    # as a hook into "pre.tex" and "colophon.tex")
    #
    if ($1==".FILE") {
        filename = $2
        strip(2)
        print >filename
        next
    }




    #
    # Handle blockquotes and verbatims/code samples
    #
    if ($1=="*")
        blockquote()
    if ($1==".")
        verbatim()
    if ($1=="=")
        description()
    endblocks()


    #
    # Copy everything else directly to output
    #
    print                                    > document
}

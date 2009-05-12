#! /usr/bin/python
# Copyright 2009 litl, LLC. All Rights Reserved.

## this file outputs sortof-indented JavaScript,
## it's semi-readable for use when testing, but
## when checking in real output, run M-x indent-region
## in js2-mode

import sys
import xml.dom.minidom
import textwrap
import re

out = sys.stdout

commentWrapper = textwrap.TextWrapper()
commentWrapper.initial_indent = '// '
commentWrapper.subsequent_indent = '// '

def getText(nodelist):
    rc = ""
    for node in nodelist:
        if node.nodeType == node.TEXT_NODE:
            rc = rc + node.data
    return rc

def getDocstring(node):
    docstrings = node.getElementsByTagName('tp:docstring')
    docstring = ''
    if len(docstrings) > 0 and docstrings[0].parentNode == node:
        docstring = getText(docstrings[0].childNodes).strip()
    return re.sub('\s+', ' ', docstring)

def methodAsString(method):
    dbusName = method.getAttribute("name")
    args = method.getElementsByTagName("arg")
    methodDocstring = getDocstring(method)
    inArgs = ''
    inArgsComments = ''
    outArgs = ''
    outArgsComments = ''
    for arg in args:
        argName = arg.getAttribute('name')
        docstring = getDocstring(arg)
        tpType = arg.getAttribute('tp:type')
        if tpType and len(tpType) > 0:
            docstring = "(" + tpType + ") " + docstring
        comment = commentWrapper.fill('  ' + argName + ': ' + docstring) + '\n'
        direction = arg.getAttribute('direction')
        type = arg.getAttribute('type')
        if direction == 'in' or (method.tagName == 'signal' and not direction):
            inArgs += type
            inArgsComments += comment
        elif direction == 'out':
            outArgs += type
            outArgsComments += comment
        else:
            raise Exception("unknown direction " + direction)

    errorComments = ''
    possibleErrors = method.getElementsByTagName("tp:possible-errors")
    if possibleErrors:
        for error in possibleErrors[0].getElementsByTagName("tp:error"):
            errorName = error.getAttribute("name")
            docstring = getDocstring(error)
            errorComments += commentWrapper.fill('  ' + errorName + ' ' + docstring) + '\n'

    if "'" in methodDocstring:
        # js2-mode gets pissy about unmatched quotes in comments,
        # it's a bug
        methodDocstring = methodDocstring.replace("'", "")
    docComment = commentWrapper.fill(dbusName + ': ' + methodDocstring) + '\n'

    if len(inArgsComments) > 0:
        inArgsComments = "// In Args:\n" + inArgsComments
    if len(outArgsComments) > 0:
        outArgsComments = "// Out Args:\n" + outArgsComments
    if len(errorComments) > 0:
        errorComments = "// Errors:\n" + errorComments

    inSig = "inSignature: '%s'" % inArgs
    outSig = "outSignature: '%s'" % outArgs

    asString = "\n%s%s%s%s\n{ name: '%s', %s, %s }" % \
        (docComment, inArgsComments, outArgsComments, errorComments,
         dbusName, inSig, outSig)
    return asString

def methodsOrSignalsAsString(methodsOrSignals):
    if len(methodsOrSignals) == 0:
        return "[]"

    asString = '[\n'
    for method in methodsOrSignals:
        asString = asString + '        ' + methodAsString(method) + ',\n'

    asString = asString[:-2] + '\n   ]'

    return asString

def propertiesAsString(properties):
    if len(properties) == 0:
        return "[]"

    asString = '[\n'
    for prop in properties:
        name = prop.getAttribute("name")
        type = prop.getAttribute("type")
        tpType = prop.getAttribute("tp:type")
        access = prop.getAttribute("access")
        docComment = name + ': '
        if tpType and len(tpType) > 0:
            docComment += '(' + tpType + ') ';
        docComment = '\n' + commentWrapper.fill(docComment + getDocstring(prop))
        asString += "%s\n\n    { name: '%s', signature: '%s', access: '%s' },\n" % \
            (docComment, name, type, access);

    asString = asString[:-2] + '\n   ]'

    return asString

def outputEnums(enums, dbusIfaceName):
    for e in enums:
        eName = e.getAttribute('name')
        out.write('\n// ' + eName + ' in ' + dbusIfaceName + '\n')
        values = []
        for val in e.getElementsByTagName('tp:enumvalue'):
            valSuffix = val.getAttribute('suffix')
            valValue = val.getAttribute('value')
            valDocstring = getDocstring(val)
            values.append((valSuffix, valValue, valDocstring))
        for val in values:
            out.write(commentWrapper.fill('  ' + val[0] + ': ' + val[2]) + '\n')
        out.write('\n')
        for val in values:
            out.write('const %s_%s = %s;\n' % (eName, val[0], val[1]))
        out.write('\n')

def outputFlags(flags, dbusIfaceName):
    for e in flags:
        eName = e.getAttribute('name')
        out.write('\n// ' + eName + ' in ' + dbusIfaceName + '\n')
        values = []
        for val in e.getElementsByTagName('tp:flag'):
            valSuffix = val.getAttribute('suffix')
            valValue = val.getAttribute('value')
            valDocstring = getDocstring(val)
            values.append((valSuffix, valValue, valDocstring))
        for val in values:
            out.write(commentWrapper.fill('  ' + val[0] + ': ' + val[2]) + '\n')
        out.write('\n')
        for val in values:
            out.write('const %s_%s = %s;\n' % (eName, val[0], val[1]))
        out.write('\n')

def outputInterface(iface):
    dbusIfaceName = iface.getAttribute("name")
    jsIfaceName = dbusIfaceName.replace('org.freedesktop.', '').replace('.','')

    methods = methodsOrSignalsAsString(iface.getElementsByTagName('method'));
    signals = methodsOrSignalsAsString(iface.getElementsByTagName('signal'));

    properties = propertiesAsString(iface.getElementsByTagName('property'));

    out.write("""
var %s = {
    name: '%s',
    methods: %s,
    signals: %s,
    properties: %s
};
""" % (jsIfaceName, dbusIfaceName, methods, signals, properties));

    enums = iface.getElementsByTagName('tp:enum')
    outputEnums(enums, dbusIfaceName);

    flags = iface.getElementsByTagName('tp:flags')
    outputFlags(flags, dbusIfaceName);

def outputNode(node):
    for child in node.childNodes:
        if child.nodeType != child.ELEMENT_NODE:
            continue
        if child.tagName == 'interface':
            outputInterface(child)
        else:
            raise Exception("unknown tag in <node>: " + child.tagName)

out.write("// AUTOMATICALLY GENERATED INTERFACE FILE\n")
out.write("// Do not edit by hand.\n\n")

for filename in sys.argv[1:]:
    if filename.endswith('nm-manager-client.xml'):
        ## nm-manager-client.xml is nm-manager.xml
        ## with some deprecated stuff removed, to
        ## generate dbus-glib client bindings. The
        ## detailed docs and stuff are in nm-manager.xml though.
        continue

    dom = xml.dom.minidom.parse(filename)

    for child in dom.childNodes:
        if child.nodeType != child.ELEMENT_NODE:
            continue
        if child.tagName == 'node':
            outputNode(child)
        elif child.tagName == 'tp:generic-types':
            enums = child.getElementsByTagName('tp:enum')
            outputEnums(enums, 'generic-types');
            flags = child.getElementsByTagName('tp:flags')
            outputFlags(flags, 'generic-types');
        elif child.tagName in ['tp:spec', 'tp:title',
                               'tp:version', 'tp:copyright',
                               'tp:license', 'xi:include',
                               'tp:errors' ]:
            continue
        else:
            raise Exception("unknown tag at root: " + child.tagName)

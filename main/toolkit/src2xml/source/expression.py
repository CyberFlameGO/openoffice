# *************************************************************
#  
#  Licensed to the Apache Software Foundation (ASF) under one
#  or more contributor license agreements.  See the NOTICE file
#  distributed with this work for additional information
#  regarding copyright ownership.  The ASF licenses this file
#  to you under the Apache License, Version 2.0 (the
#  "License"); you may not use this file except in compliance
#  with the License.  You may obtain a copy of the License at
#  
#    http://www.apache.org/licenses/LICENSE-2.0
#  
#  Unless required by applicable law or agreed to in writing,
#  software distributed under the License is distributed on an
#  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
#  KIND, either express or implied.  See the License for the
#  specific language governing permissions and limitations
#  under the License.
#  
# *************************************************************


import sys
import globals

def toString (node):

    if node == None:
        return ''

    chars = '('

    if type(node.left) == type(0):
        chars += "%d"%node.left
    else:
        chars += toString(node.left)

    chars += node.op

    if type(node.right) == type(0):
        chars += "%d"%node.right
    else:
        chars += toString(node.right)

    chars += ")"

    return chars

class Node(object):
    def __init__ (self):
        self.left = None
        self.right = None
        self.parent = None
        self.op = None

class ExpParser(object):

    def __init__ (self, tokens):
        self.tokens = tokens

    def jumpToRoot (self):
        while self.ptr.parent != None:
            self.ptr = self.ptr.parent

    def build (self):
        self.ptr = Node()

        for token in self.tokens:

            if token in '+-':
                if self.ptr.left == None:
                    raise globals.ParseError ('')
                if self.ptr.right == None:
                    self.ptr.op = token
                else:
                    self.jumpToRoot()
                    self.ptr.parent = Node()
                    self.ptr.parent.left = self.ptr
                    self.ptr = self.ptr.parent
                    self.ptr.op = token

            elif token in '*/':
                if self.ptr.left == None:
                    raise globals.ParseError ('')
                elif self.ptr.right == None:
                    self.ptr.op = token
                else:
                    num = self.ptr.right
                    self.ptr.right = Node()
                    self.ptr.right.parent = self.ptr
                    self.ptr.right.left = num
                    self.ptr.right.op = token
                    self.ptr = self.ptr.right

            elif token == '(':
                if self.ptr.left == None:
                    self.ptr.left = Node()
                    self.ptr.left.parent = self.ptr
                    self.ptr = self.ptr.left
                elif self.ptr.right == None:
                    self.ptr.right = Node()
                    self.ptr.right.parent = self.ptr
                    self.ptr = self.ptr.right
                else:
                    raise globals.ParseError ('')

            elif token == ')':
                if self.ptr.left == None:
                    raise globals.ParseError ('')
                elif self.ptr.right == None:
                    raise globals.ParseError ('')
                elif self.ptr.parent == None:
                    pass
                else:
                    self.ptr = self.ptr.parent

            else:
                num = int(token)
                if self.ptr.left == None:
                    self.ptr.left = num
                elif self.ptr.right == None:
                    self.ptr.right = num
                else:
                    raise globals.ParseError ('')

    def dumpTree (self):
        self.jumpToRoot()
        print toString(self.ptr)





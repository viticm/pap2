using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;

namespace XSJ.Sword3.Editor.Studio.Component
{
    class ItemCollection : ArrayList
    {
        public ItemCollection() : base() { }
        public ItemCollection(ICollection c) : base(c) { }
    }
}

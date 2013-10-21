/*===================================
 * Purpose:
 * Created By: WilsonWei
 * Created On: 2007-2-7
 * (c)Kingsoft Corporation
 * Comments:
 * 
 * 
 * 
 * ====================================*/
using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;

namespace XSJ.Sword3.Editor.Studio.Component
{
    class ShopListCollection : ArrayList
    {
        public ShopListCollection() : base() { }
        public ShopListCollection(ICollection c) : base(c) { }
    }
}

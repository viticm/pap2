//    Copyright (C) 2005  Sebastian Faltoni
//
//    This library is free software; you can redistribute it and/or
//    modify it under the terms of the GNU Lesser General Public
//    License as published by the Free Software Foundation; either
//    version 2.1 of the License, or (at your option) any later version.
//
//    This library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public
//    License along with this library; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


using System;
using System.Collections.Generic;
using System.Text;

namespace Fireball.Windows.Forms
{
    public static class DiscoverLanguage
    {
        #region Declares
        private static string _AddRemoveButtons;

        private static string _ConfigureButtons;

        private static string _FewerButtons;

        private static string _MoreButtons;

        private static string _PaneOptions; 
        #endregion


        static DiscoverLanguage()
        {
            DiscoverLanguage._AddRemoveButtons = "&Add/Remove Buttons";


            DiscoverLanguage._ConfigureButtons = "Configure Buttons";

            DiscoverLanguage._FewerButtons = "View Fewer Buttons";

            DiscoverLanguage._MoreButtons = "View More Buttons";

            DiscoverLanguage._PaneOptions = "Discover Options"; 

        }

        public static string PaneOptions
        {
            get { return DiscoverLanguage._PaneOptions; }
            set { DiscoverLanguage._PaneOptions = value; }
        }

        public static string MoreButtons
        {
            get { return DiscoverLanguage._MoreButtons; }
            set { DiscoverLanguage._MoreButtons = value; }
        }

        public static string FewerButtons
        {
            get { return DiscoverLanguage._FewerButtons; }
            set { DiscoverLanguage._FewerButtons = value; }
        }

        public static string ConfigureButtons
        {
            get { return DiscoverLanguage._ConfigureButtons; }
            set { DiscoverLanguage._ConfigureButtons = value; }
        }

        public static string AddRemoveButtons
        {
            get { return DiscoverLanguage._AddRemoveButtons; }
            set { DiscoverLanguage._AddRemoveButtons = value; }
        }

    }
}

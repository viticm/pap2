// SystemColors.cs
//
// Authors:
//   Sebastian Faltoni
//
// Copyright (C) 2006 DotNetFireball (http://www.dotnetfireball.net)
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
// 
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//


using System;
using System.Collections.Generic;
using System.Text;

namespace Fireball.Cairo
{
    public sealed class SystemColors
    {
        // Default Windows XP color table
        static private Color active_border = Color.FromArgbSystem(255, 212, 208, 200, "ActiveBorder", KnownColor.ActiveBorder);
        static private Color active_caption = Color.FromArgbSystem(255, 0, 164, 164, "ActiveCaption", KnownColor.ActiveCaption);
        static private Color active_caption_text = Color.FromArgbSystem(255, 255, 255, 255, "ActiveCaptionText", KnownColor.ActiveCaptionText);
        static private Color app_workspace = Color.FromArgbSystem(255, 128, 128, 128, "AppWorkspace", KnownColor.AppWorkspace);
        static private Color control = Color.FromArgbSystem(255, 212, 208, 200, "Control", KnownColor.Control);
        static private Color control_dark = Color.FromArgbSystem(255, 128, 128, 128, "ControlDark", KnownColor.ControlDark);
        static private Color control_dark_dark = Color.FromArgbSystem(255, 64, 64, 64, "ControlDarkDark", KnownColor.ControlDarkDark);
        static private Color control_light = Color.FromArgbSystem(255, 212, 208, 200, "ControlLight", KnownColor.ControlLight);
        static private Color control_light_light = Color.FromArgbSystem(255, 255, 255, 255, "ControlLightLight", KnownColor.ControlLightLight);
        static private Color control_text = Color.FromArgbSystem(255, 0, 0, 0, "ControlText", KnownColor.ControlText);
        static private Color desktop = Color.FromArgbSystem(255, 0, 0, 0, "Desktop", KnownColor.Desktop);
        static private Color gray_text = Color.FromArgbSystem(255, 128, 128, 128, "GrayText", KnownColor.GrayText);
        static private Color highlight = Color.FromArgbSystem(255, 10, 36, 106, "Highlight", KnownColor.Highlight);
        static private Color highlight_text = Color.FromArgbSystem(255, 255, 255, 255, "HighlightText", KnownColor.HighlightText);
        static private Color hot_track = Color.FromArgbSystem(255, 0, 0, 128, "HotTrack", KnownColor.HotTrack);
        static private Color inactive_border = Color.FromArgbSystem(255, 212, 208, 200, "InactiveBorder", KnownColor.InactiveBorder);
        static private Color inactive_caption = Color.FromArgbSystem(255, 128, 128, 128, "InactiveCaption", KnownColor.InactiveCaption);
        static private Color inactive_caption_text = Color.FromArgbSystem(255, 212, 208, 200, "InactiveCaptionText", KnownColor.InactiveCaptionText);
        static private Color info = Color.FromArgbSystem(255, 255, 255, 225, "Info", KnownColor.Info);
        static private Color info_text = Color.FromArgbSystem(255, 0, 0, 0, "InfoText", KnownColor.InfoText);
        static private Color menu = Color.FromArgbSystem(255, 212, 208, 200, "Menu", KnownColor.Menu);
        static private Color menu_text = Color.FromArgbSystem(255, 0, 0, 0, "MenuText", KnownColor.MenuText);
        static private Color scroll_bar = Color.FromArgbSystem(255, 212, 208, 200, "ScrollBar", KnownColor.ScrollBar);
 
        static private Color window = Color.FromArgbSystem(255, 255, 255, 255, "Window", KnownColor.Window);
        static private Color window_frame = Color.FromArgbSystem(255, 0, 0, 0, "WindowFrame", KnownColor.WindowFrame);
        static private Color window_text = Color.FromArgbSystem(255, 0, 0, 0, "WindowText", KnownColor.WindowText);
    }
}

//ORIGINAL LGPL SOURCE CODE FINDED ON COMPONA LGPL SOURCE CODE

using Fireball.Drawing.GDI;
using Fireball.Windows.Forms.CodeEditor.TextDraw;
using System;

namespace Fireball.Windows.Forms.CodeEditor
{
	/// <summary>
	/// Represents which split view is currently active in the syntaxbox
	/// </summary>
	public enum ActiveView
	{
		TopLeft,
		TopRight,
		BottomLeft,
		BottomRight,
	}

	/// <summary>
	/// Indent styles used by the control
	/// </summary>
	public enum IndentStyle
	{
		/// <summary>
		/// Caret is always confined to the first column when a new line is inserted
		/// </summary>
		None = 0,
		/// <summary>
		/// New lines inherit the same indention as the previous row.
		/// </summary>
		LastRow = 1,
		/// <summary>
		/// New lines get their indention from the scoping level.
		/// <seealso cref="CauseIndent"/>
		/// </summary>
		Scope = 2,
		/// <summary>
		/// New lines get thir indention from the scoping level or from the previous row
		/// depending on which is most indented.
		/// <seealso cref="CauseIndent"/>
		/// </summary>
		Smart = 3,
	}
}

namespace Fireball.Windows.Forms.CodeEditor.TextDraw
{
	/// <summary>
	/// To be implemented
	/// </summary>
	public enum TextDrawType
	{
		/// <summary>
		/// For public use only
		/// </summary>
		StarBorder = 0,
		/// <summary>
		/// For public use only
		/// </summary>
		MinusBorder = 1,
		/// <summary>
		/// For public use only
		/// </summary>
		DoubleBorder = 2,
		/// <summary>
		/// For public use only
		/// </summary>
		SingleBorder = 3
	}

	/// <summary>
	/// For public use only
	/// </summary>
	public enum TextDrawDirectionType
	{
		/// <summary>
		/// For public use only
		/// </summary>
		Right = 1,
		/// <summary>
		/// For public use only
		/// </summary>
		Left = 2,
		/// <summary>
		/// For public use only
		/// </summary>
		Up = 4,
		/// <summary>
		/// For public use only
		/// </summary>
		Down = 8
	}


	/// <summary>
	/// For public use only
	/// </summary>
	public enum TextBorderChars
	{
		/// <summary>
		/// For public use only
		/// </summary>
		DownRight = 0,
		/// <summary>
		/// For public use only
		/// </summary>
		RightLeft = 1,
		/// <summary>
		/// For public use only
		/// </summary>
		DownRightLeft = 2,
		/// <summary>
		/// For public use only
		/// </summary>
		DownLeft = 4,
		/// <summary>
		/// For public use only
		/// </summary>
		DownUp = 5,
		/// <summary>
		/// For public use only
		/// </summary>
		DownUpRight = 10,
		/// <summary>
		/// For public use only
		/// </summary>
		DownUpRightLeft = 11,
		/// <summary>
		/// For public use only
		/// </summary>
		DownUpLeft = 12,
		/// <summary>
		/// For public use only
		/// </summary>
		UpRight = 20,
		/// <summary>
		/// For public use only
		/// </summary>
		UpRightLeft = 21,
		/// <summary>
		/// For public use only
		/// </summary>
		UpLeft = 22,
		/// <summary>
		/// For public use only
		/// </summary>
		Blank = 6
	}

	/// <summary>
	/// Text actions that can be performed by the SyntaxBoxControl
	/// </summary>
	public enum XTextAction
	{
		/// <summary>
		/// The control is not performing any action
		/// </summary>
		xtNone = 0,
		/// <summary>
		/// The control is in Drag Drop mode
		/// </summary>
		xtDragText = 1,
		/// <summary>
		/// The control is selecting text
		/// </summary>
		xtSelect = 2
	}
}

namespace Fireball.Windows.Forms.CodeEditor.Painter
{
	/// <summary>
	/// View point struct used by the SyntaxBoxControl.
	/// The struct contains information about various rendering parameters that the IPainter needs.
	/// </summary>
	public class ViewPoint
	{
        private int _YOffset;
        private int _RowHeight;
        private int _CharWidth;
        private int _FirstVisibleRow;
        private int _VisibleRowCount;
        private int _FirstVisibleColumn;
        private int _ClientAreaWidth;
        private int _ClientAreaStart;
        private XTextAction _Action;
        private int _GutterMarginWidth;
        private int _LineNumberMarginWidth;
        private int _TotalMarginWidth;
        private int _TextMargin;

		/// <summary>
		/// Used for offsetting the screen in y axis.
		/// </summary>
        public int YOffset
        {
            get { return _YOffset; }
            set { _YOffset = value; }
        }

		/// <summary>
		/// Height of a row in pixels
		/// </summary>
        public int RowHeight
        {
            get { return _RowHeight; }
            set { _RowHeight = value; }
        }

        /// <summary>
        /// Width of a char (space) in pixels
        /// </summary>
        public int CharWidth
        {
            get { return _CharWidth; }
            set { _CharWidth = value; }
        }

		/// <summary>
		/// Index of the first visible row
		/// </summary>
        public int FirstVisibleRow
        {
            get { return _FirstVisibleRow; }
            set { _FirstVisibleRow = value; }
        }
        
		/// <summary>
		/// Number of rows that can be displayed in the current view
		/// </summary>
        public int VisibleRowCount
        {
            get { return _VisibleRowCount; }
            set { _VisibleRowCount = value; }
        }

		/// <summary>
		/// Index of the first visible column
		/// </summary>     
        public int FirstVisibleColumn
        {
            get { return _FirstVisibleColumn; }
            set { _FirstVisibleColumn = value; }
        }

		/// <summary>
		/// Width of the client area in pixels
		/// </summary>
        public int ClientAreaWidth
        {
            get { return _ClientAreaWidth; }
            set { _ClientAreaWidth = value; }
        }

		/// <summary>
		/// Height of the client area in pixels
		/// </summary>
        public int ClientAreaStart
        {
            get { return _ClientAreaStart; }
            set { _ClientAreaStart = value; }
        }

		/// <summary>
		/// The action that the SyntaxBoxControl is currently performing
		/// </summary>
        public XTextAction Action
        {
            get { return _Action; }
            set { _Action = value; }
        }

		/// <summary>
		/// Width of the gutter margin in pixels
		/// </summary>
        public int GutterMarginWidth
        {
            get { return _GutterMarginWidth; }
            set { _GutterMarginWidth = value; }
        }

		/// <summary>
		/// Width of the Linenumber margin in pixels
		/// </summary>
        public int LineNumberMarginWidth
        {
            get { return _LineNumberMarginWidth; }
            set { _LineNumberMarginWidth = value; }
        }

		/// <summary>
		/// 
		/// </summary>
        public int TotalMarginWidth
        {
            get { return _TotalMarginWidth; }
            set { _TotalMarginWidth = value; }
        }

		/// <summary>
		/// Width of the text margin (sum of gutter + linenumber + folding margins)
		/// </summary>
        public int TextMargin
        {
            get { return _TextMargin; }
            set { _TextMargin = value; }
        }

		//document items
	}


	/// <summary>
	/// Struct used by the Painter_GDI class.
	/// </summary>
	public class RenderItems : IDisposable
	{
		/// <summary>
		/// For public use only
		/// </summary>
		public GDISurface BackBuffer; //backbuffer surface
		/// <summary>
		/// For public use only
		/// </summary>
		public GDISurface SelectionBuffer; //backbuffer surface
		/// <summary>
		/// For public use only
		/// </summary>
		public GDISurface StringBuffer; //backbuffer surface
		/// <summary>
		/// For public use only
		/// </summary>
		public GDIFont FontNormal; //Font , no decoration		
		/// <summary>
		/// For public use only
		/// </summary>
		public GDIFont FontBold; //Font , bold
		/// <summary>
		/// For public use only
		/// </summary>
		public GDIFont FontItalic; //Font , italic
		/// <summary>
		/// For public use only
		/// </summary>
		public GDIFont FontBoldItalic; //Font , bold & italic
		/// <summary>
		/// For public use only
		/// </summary>
		public GDIFont FontUnderline; //Font , no decoration		
		/// <summary>
		/// For public use only
		/// </summary>
		public GDIFont FontBoldUnderline; //Font , bold
		/// <summary>
		/// For public use only
		/// </summary>
		public GDIFont FontItalicUnderline; //Font , italic
		/// <summary>
		/// For public use only
		/// </summary>
		public GDIFont FontBoldItalicUnderline; //Font , bold & italic
		/// <summary>
		/// For public use only
		/// </summary>
		public GDIBrush GutterMarginBrush; //Gutter magrin brush
		/// <summary>
		/// For public use only
		/// </summary>
		public GDIBrush GutterMarginBorderBrush; //Gutter magrin brush
		/// <summary>
		/// For public use only
		/// </summary>
		public GDIBrush LineNumberMarginBrush; //linenumber margin brush
		/// <summary>
		/// For public use only
		/// </summary>
		public GDIBrush LineNumberMarginBorderBrush; //linenumber margin brush
		/// <summary>
		/// For public use only
		/// </summary>
		public GDIBrush BackgroundBrush; //background brush
		/// <summary>
		/// For public use only
		/// </summary>
		public GDIBrush HighLightLineBrush; //background brush
		/// <summary>
		/// For public use only
		/// </summary>
		public GDIBrush OutlineBrush; //background brush

        #region IDisposable Members

        public void Dispose()
        {
            if (BackBuffer != null)
            {
                BackBuffer.Dispose();
                BackBuffer = null;
            }
            if (SelectionBuffer != null)
            {
                SelectionBuffer.Dispose();
                SelectionBuffer = null;
            }
            if (StringBuffer != null)
            {
                StringBuffer.Dispose();
                StringBuffer = null;
            }
            if (FontNormal != null)
            {
                FontNormal.Dispose();
                FontNormal = null;
            }
            if (FontBold != null)
            {
                FontBold.Dispose();
                FontBold = null;
            }
            if (FontItalic != null)
            {
                FontItalic.Dispose();
                FontItalic = null;
            }
            if (FontBoldItalic != null)
            {
                FontBoldItalic.Dispose();
                FontBoldItalic = null;
            }
            if (FontUnderline != null)
            {
                FontUnderline.Dispose();
                FontUnderline = null;
            }
            if (FontBoldUnderline != null)
            {
                FontBoldUnderline.Dispose();
                FontBoldUnderline = null;
            }
            if (FontItalicUnderline != null)
            {
                FontItalicUnderline.Dispose();
                FontItalicUnderline = null;
            }
            if (FontBoldItalicUnderline != null)
            {
                FontBoldItalicUnderline.Dispose();
                FontBoldItalicUnderline = null;
            }
            if (GutterMarginBrush != null)
            {
                GutterMarginBrush.Dispose();
                GutterMarginBrush = null;
            }
            if (GutterMarginBrush != null)
            {
                GutterMarginBrush.Dispose();
                GutterMarginBrush = null;
            }
            if (LineNumberMarginBrush != null)
            {
                LineNumberMarginBrush.Dispose();
                LineNumberMarginBrush = null;
            }
            if (LineNumberMarginBorderBrush != null)
            {
                LineNumberMarginBorderBrush.Dispose();
                LineNumberMarginBorderBrush = null;
            }
            if (BackgroundBrush != null)
            {
                BackgroundBrush.Dispose();
                BackgroundBrush = null;
            }
            if (HighLightLineBrush != null)
            {
                HighLightLineBrush.Dispose();
                HighLightLineBrush = null;
            }
            if (OutlineBrush != null)
            {
                OutlineBrush.Dispose();
                OutlineBrush = null;
            }
        }

        #endregion
    }
}
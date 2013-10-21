#region Copyright
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
#endregion

using System;
using System.Drawing.Printing;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.IO;

namespace Fireball.Printing
{
    public class DefaultPrintingContext : BasePrintingContext
    {

        protected PrintDocument printDocument = null;

        protected PrintableDocument printableDocument;

        protected PrintPageEventArgs printArgs;

        public DefaultPrintingContext(PrintableDocument printableDocument)
        {
            this.CreatePrintDocument();
            this.printableDocument = printableDocument;
        }

        protected virtual void CreatePrintDocument()
        {
            printDocument = new PrintDocument();         

            printDocument.PrintPage += new PrintPageEventHandler(printDocument_PrintPage);

            printDocument.BeginPrint += new PrintEventHandler(printDocument_BeginPrint);
        }

        void printDocument_BeginPrint(object sender, PrintEventArgs e)
        {
            Reset();
        }


         
        void printDocument_PrintPage(object sender, PrintPageEventArgs e)
        {
            printArgs = e;

            precalculateSize();

            dontDrawNow = false;

            e.Graphics.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.AntiAlias;
            e.Graphics.TextRenderingHint = System.Drawing.Text.TextRenderingHint.AntiAlias;

            if (_CurrentElement == null)
            {
                if (chapterIndex < this.printableDocument.Childs.Count)
                {
                    _CurrentElement = this.printableDocument.Childs[chapterIndex];
                }
                else
                {
                    e.HasMorePages = false;
                    return;
                }
            }

            _CurrentElement.Render(this);
        }

        int chapterIndex = 0;

        PrintElementBase _CurrentElement = null;


        public PrintDocument PrintDocument
        {
            get
            {
                return printDocument;
            }
        }

        public override void PrintParagraph(Paragraph paragraph)
        {
            if (dontDrawNow)
                return;
        }

        public override void PrintTextBlock(TextBlock textBlock)
        {
            if (dontDrawNow)
                return;

            DrawBlock(textBlock);
        }

        private Rect Rectangle2Rect(RectangleF rect)
        {
            return new Rect(rect.X, rect.Y, 
                rect.Width, rect.Height);
        }

        private RectangleF Rect2Rectangle(Rect rect)
        {
            return new RectangleF(rect.X, rect.Y, 
                rect.Width, rect.Height);
        }

        private void DrawBlock(Block block)
        {
            RectangleF bounds = new RectangleF(this.CurrentX, this.CurrentY, 0, 0);

            if (block.Content is String)
            {
                Rect b = block.Bounds;

                PrintElementBase parent = (PrintElementBase)block.Parent;
                b.Width = printArgs.PageBounds.Width - parent.Bounds.Width ;
                b.Height = parent.Bounds.Height -this.CurrentY;
                block.Bounds = b;

                DrawContentString(block);

                bounds = this.Rect2Rectangle(block.Bounds);

                return;
            }

            if (block.Childs.Count > 0)
            {
                PrintElementBase prev = null;

                foreach (PrintElementBase current in block.Childs)
                {
                    if (current is LineBreak)
                    {
                        DrawLineBreak((LineBreak)current,prev);
                    }
                    else
                        current.Render(this);

                    prev = current;

                    if (bounds.Right < current.Bounds.Right)
                    {
                        bounds.Width = (current.Bounds.Right - bounds.Right) - bounds.Left;
                    }

                    if (bounds.Bottom < current.Bounds.Bottom)
                    {
                        bounds.Height = (current.Bounds.Bottom - bounds.Bottom) - bounds.Top;
                    }

                    block.Bounds = this.Rectangle2Rect(bounds);
                }
            }

            block.Bounds = this.Rectangle2Rect(bounds);

            this.currentY = bounds.Bottom;

            this.currentX = bounds.Right; 
        }

        void DrawLineBreak(LineBreak breakElement,PrintElementBase prevElement)
        {
            
           
            if (prevElement != null)
            {
                this.currentY = prevElement.Bounds.Bottom + breakElement.BreakHeight;
            }
            else
            {
                this.currentY += breakElement.BreakHeight;
            }

            breakElement.Bounds = new Rect(this.CurrentX, this.currentY, 
                printArgs.PageBounds.Width - this.CurrentX, breakElement.BreakHeight);

            this.currentX = 0;
        }

        public override float CurrentX
        {
            get
            {
                if (base.currentX >= printArgs.PageBounds.Right)
                    base.currentX = printArgs.PageBounds.Left;

                if (base.currentX <= printArgs.PageBounds.Left)
                    base.currentX = printArgs.PageBounds.Left;

                return base.CurrentX;
            }
        }

        public override void PrintRawData(params object[] datas)
        {
            if (dontDrawNow)
                return;
        }

        public override void PrintSpanElement(SpanElement spanElement)
        {
            if (dontDrawNow)
                return;
        }

        public override void PrintPrintableDocument(PrintableDocument printableDocument)
        {
            if (dontDrawNow)
                return;
        }

        public override void PrintChapter(Chapter chapter)
        {
            if (dontDrawNow)
                return;

            DrawBlock(chapter);
        }

        public override void PrintImageElement(ImageElement imageElement)
        {
            if (dontDrawNow || imageElement.SourceStream == Stream.Null)
                return;

            using (Bitmap bitmap = new Bitmap(imageElement.SourceStream))
            {
                if (imageElement.Width == 0)
                    imageElement.Width = bitmap.Width;

                if (imageElement.Height == 0)
                    imageElement.Height = bitmap.Height;

                RectangleF bounds = new RectangleF(this.CurrentX, this.CurrentY,
                    imageElement.Width, imageElement.Height);

                printArgs.Graphics.DrawImage(bitmap, bounds);

                imageElement.Bounds = this.Rectangle2Rect(bounds);

              //  this.currentY = bounds.Bottom;
                this.currentX = bounds.Right;
            } 
        }

        bool dontDrawNow = false;

        void DrawContentElement(IPrintElement element)
        {
            element.Render(this);
        }

        void DrawContentString(TextElement element)
        {
            //Implement TextWrap

            Font font = this.GetSDFontFromElement(element);

            Color fontColor = element.Foreground.ToSystemDrawingColor();

            StringFormat fmt = new StringFormat();             

            if (element is Block)
            {
                fmt = this.GetFromBlock((Block)element);
            }

            SizeF textSize = printArgs.Graphics.MeasureString(element.Content.ToString(),
                font, (int)(this.printArgs.PageBounds.Width - this.currentX), fmt);

            if (textSize.Width < element.Bounds.Width)
            {
                textSize.Width = element.Bounds.Width;
            }

            if (textSize.Height < element.Bounds.Height)
            {
                textSize.Height = element.Bounds.Height;
            }

            RectangleF drawRect = new RectangleF(new PointF(this.CurrentX,
                this.CurrentY), textSize);

            element.Bounds = this.Rectangle2Rect(drawRect);

            if (element.Bounds.Bottom >= printArgs.PageBounds.Bottom)
            {
                printArgs.HasMorePages = true;

                _CurrentElement = element;

                dontDrawNow = true;

                return;
            }

            printArgs.Graphics.DrawString(element.Content.ToString(),
                 font,
                 new SolidBrush(fontColor),
                 this.Rect2Rectangle(element.Bounds), fmt);

            this.currentY += element.Bounds.Height;

            this.currentX = printArgs.MarginBounds.Left;
        }

        private StringFormat GetFromBlock(Block block)
        {
            StringFormat fmt = new StringFormat();

            if (block.VerticalContentAlignment == VerticalAlignment.Top)
            {
                fmt.LineAlignment = StringAlignment.Near;
            }
            else if (block.VerticalContentAlignment == VerticalAlignment.Middle)
            {
                fmt.LineAlignment = StringAlignment.Center;
            }
            else
            {
                fmt.LineAlignment = StringAlignment.Far;
            }

            return fmt;
        }

        private Font GetSDFontFromElement(TextElement element)
        {
            GraphicsUnit unit = printArgs.Graphics.PageUnit;

            if(unit == GraphicsUnit.Display)
            {
                unit = GraphicsUnit.Millimeter;
            }

            return new Font(element.FontName, element.FontSize
                , (System.Drawing.FontStyle)Enum.Parse(
                typeof(System.Drawing.FontStyle),
                element.FontStyle.ToString()),
                unit);
        }

        public void Print()
        {
            this.PrintDocument.Print();

            foreach (Chapter current in this.printableDocument.Childs)
            {
                this.PrintChapter(current);
            }
        }

        void Reset()
        {
            foreach (PrintElementBase current in this.printableDocument.Childs)
            {                
                Reset(current);
            }
        }

        void Reset(PrintElementBase element)
        {
            this.currentX = 0;
            this.currentY = 0;

            element.Bounds = Rect.Empty;

            IHaveChild haveChilds = element as IHaveChild;

            if (haveChilds != null)
            {
                foreach (PrintElementBase current in haveChilds.Childs)
                {
                    Reset(current);
                }
            }
        }

        void precalculateSize()
        {
            IList<FlowRow> rows = this.GetRows();

            foreach (FlowRow row in rows)
            {
                getRowBounds(row);
            }
        }

        private Rect getRowBounds(FlowRow row)
        {
            foreach (FlowRowElement element in row.Elements)
            {
                Rect rectElement = GetRect(element.Element);

                foreach (FlowRow childRow in element.ChildsRows)
                {
                    Rect rect = getRowBounds(childRow);

                    rectElement.Extend(rectElement);
                }
            }

            return Rect.Empty;
        }

        private IList<FlowRow> GetRows()
        {
            FlowRowElement fakeElement = new FlowRowElement(null);

            getRows(ref fakeElement, (IHaveChild)this.printableDocument);

            return fakeElement.ChildsRows;
        }

        void getRows(ref FlowRowElement parentElement, IHaveChild block)
        {
            FlowRow currentRow = new FlowRow();

            foreach (PrintElementBase current in block.Childs)
            {
                FlowRowElement currentRowElement = new FlowRowElement(current);

                if (current is LineBreak)
                {
                    parentElement.ChildsRows.Add(currentRow);
                    currentRow = new FlowRow();
                    currentRow.Elements.Add(currentRowElement);
                    parentElement.ChildsRows.Add(currentRow);
                    currentRow = new FlowRow();
                }
                else
                {
                    currentRow.Elements.Add(currentRowElement);

                    if (current is IHaveChild)
                    {
                        getRows(ref currentRowElement, (IHaveChild)current);
                    }
                }
            }
        }

        private SizeF StringRect(string s, StringFormat sf, Rectangle destBounds, Font font)
        {
            SizeF size = printArgs.Graphics.MeasureString(s, font, destBounds.Size, sf);

            return size;
        }

        protected override Rect GetRect(PrintElementBase element)
        {
            if (element is TextBlock)
            {
                TextBlock block = (TextBlock)element;

                if (block.Content != null)
                {

                    StringFormat fmt = GetFromBlock(block);

                    Font font = GetSDFontFromElement(block);

                    SizeF stringSize = this.StringRect(block.Content.ToString(), fmt,
                        Rectangle.Empty, font);

                    block.Bounds.Extend(new Rect(0, 0, stringSize.Width, stringSize.Height));
                }
            }
            else if (element is ImageElement)
            {
                ImageElement image = (ImageElement)element;

                if (image.SourceStream != Stream.Null)
                {
                    using (Bitmap bmp = new Bitmap(image.SourceStream))
                    {
                        Rect rc = new Rect(0, 0, bmp.Width, bmp.Height);

                        image.Bounds = rc;
                    }
                }
            }

            return Rect.Empty;
        }
    }
}

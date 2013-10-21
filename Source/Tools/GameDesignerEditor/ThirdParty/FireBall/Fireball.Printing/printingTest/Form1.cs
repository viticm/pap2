using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Fireball.Printing;
using System.IO;

namespace printingTest
{
    public partial class Form1 : Form
    {
        PrintableDocument doc = new PrintableDocument();
        BasePrintingContext context = null;

        public Form1()
        {
            InitializeComponent();

            Chapter chapter = new Chapter();

            doc.Childs.Add(chapter);


            TextBlock mainBlock = new TextBlock();

            chapter.Childs.Add(mainBlock);

            ImageElement image = new ImageElement();

            MemoryStream mem = new MemoryStream();

            Properties.Resources.moto.Save(mem, System.Drawing.Imaging.ImageFormat.Png);

            image.SourceStream = mem;

            image.Width = 200;
            image.Height = 200;

            mainBlock.Childs.Add(image);


            TextBlock titleBlock = new TextBlock();

            titleBlock.Content = "Hello,World";

            titleBlock.VerticalContentAlignment = VerticalAlignment.Bottom;

            mainBlock.Childs.Add(titleBlock);

            LineBreak brk = new LineBreak();

            chapter.Childs.Add(brk);


            TextBlock breakedBlock = new TextBlock();

            breakedBlock.Content = "Sotto Hello,World";

            chapter.Childs.Add(breakedBlock);


            context = new DefaultPrintingContext(doc);

            printPreviewControl1.Document = context.PrintDocument;
 
        }

        private void toolStripButton1_Click(object sender, EventArgs e)
        {
            PrintPreviewDialog dlg = new PrintPreviewDialog();

            dlg.Document = context.PrintDocument;

            dlg.ShowDialog();
        }
    }
}
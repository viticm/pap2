using System;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.ComponentModel;

using Fireball.Collections.Generic;

namespace Fireball.Windows.Forms
{
    [ToolboxItem(true)]
    public class NumericBox:Control 
    {
        private bool _onError;
        private NumericTextBox _numBox;
        private LightCollection<NumericRange> _ranges;
        private bool _hasComma;
        private bool _hasNumbers;
        private bool _onFlashing;
        private int _flashCount;
        private bool _lastError;
        private int _decimalPlace;
        private decimal _value;
        public bool _allowError;

        private System.Timers.Timer _timer;

        public NumericBox()
        {
            this.SetStyle(
                ControlStyles.AllPaintingInWmPaint | 
                ControlStyles.OptimizedDoubleBuffer | 
                ControlStyles.ResizeRedraw | 
                ControlStyles.EnableNotifyMessage|
                ControlStyles.UserPaint
                , true);

            _onError = false;
            _hasComma = false;
            _hasNumbers = false;
            _onFlashing = false;
            _flashCount = 0;
            _value = new decimal();

            _allowError = true;

            _timer = new System.Timers.Timer(25);
            _timer.Elapsed+=new System.Timers.ElapsedEventHandler(_timer_Elapsed);

            _ranges = new  LightCollection<NumericRange>();

            _numBox = new NumericTextBox(this);
            _numBox.BorderStyle = BorderStyle.None;
            _numBox.Multiline = false;

            _numBox.TextChanged += new EventHandler(_textBox_TextChanged);
            _numBox.KeyPress += new KeyPressEventHandler(_textBox_KeyPress);

            this.Height = _numBox.Height + 4;
            this.Width = _numBox.Width + 2;

            _numBox.Location = new Point(1, 1);

            this.Controls.Add(_numBox);
        }

        private void _textBox_TextChanged(object sender, EventArgs e)
        {
            if (_numBox.TextLength == 0)
            {
                _hasNumbers = false;
                _hasComma = false;
            }

            _hasComma = _numBox.Text.Contains(",");

            _onError = !Validate();
            this.Invalidate();

            base.OnTextChanged(e);
        }
        private void _textBox_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar == '\b')
            {
                e.Handled = false;
                //TODO: verificare se cancellerà numeri o virgola
                return;
            }

            if (e.KeyChar == '.')
            {
                e.KeyChar = ',';
            }

            if (this.ValidateChar(e.KeyChar, ref _hasComma, ref _hasNumbers))
            {
                e.Handled = false;
            }
            else
            {
                e.Handled = true;
                FlashWindow();
            }
        }
        private void _timer_Elapsed(object sender, System.Timers.ElapsedEventArgs e)
        {
            if (_flashCount < 6)
            {
                _onError = !_onError;
                this.Invalidate();
                _flashCount++;
            }
            else
            {
                _flashCount = 0;
                _timer.Enabled = false;
                _onFlashing = false;
                Validate();
            }

        }

        protected override void OnSizeChanged(EventArgs e)
        {
            _numBox.Width = this.Width - 2;
            this.Height = _numBox.Height + 4;
        }
        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);

            Rectangle myFillRect = new Rectangle(0, 0, this.Width, this.Height);
            Rectangle myDrawRect = new Rectangle(0, 0, this.Width - 1, this.Height - 1);

            e.Graphics.FillRectangle(SystemBrushes.Window, myFillRect);
            
            if (_onError)
            {
                e.Graphics.DrawRectangle(Pens.Red, myDrawRect);
            }
            else
            {
                e.Graphics.DrawRectangle(SystemPens.ControlDark, myDrawRect);
            }
        }

        private void FlashWindow()
        {
            if (!_onFlashing)
            {
                _onFlashing = true;

                _lastError = _onError;
                _timer.Enabled = true;

            }
        }
        private bool Validate()
        {
            if (_numBox.Text.Length == 0) return true;

            string str = _numBox.Text;
            int decimalCount = 0;
            bool valid = false;

            if ((_decimalPlace == 0) && _hasComma)
            {
                return false;
            }

            if(_hasComma)
            {
                int commaIndex = str.IndexOf(',');
                if (commaIndex == (str.Length - 1)) // la virgola è l'ultimo carattere
                {
                    return false;
                }
                else
                {
                    decimalCount = str.Length - 1 - commaIndex;
                    if (!(decimalCount <= _decimalPlace)) return false;
                }
            }

            bool parsed = decimal.TryParse(str,out _value);

            if (parsed)
            {
                if (_ranges.Count > 0)
                {
                    for (int i = 0; i < _ranges.Count; i++)
                    {
                        valid |= _ranges[i].Validate(_value);
                    }
                }
                else
                {
                    valid = (_value >= decimal.MinValue) && (_value <= decimal.MaxValue);
                }
            }
            else
            {
                valid = false;
            }

            return valid;
        }
        private bool ValidateChar(char vchar, ref bool hasComma, ref bool hasNumbers)
        {
            bool valid = false;
            if ((vchar == ',') && _decimalPlace==0)
            {
                return false;
            }

            if ((vchar == ',') && hasNumbers)
            {
                if (!hasComma)
                {
                    valid = true;
                    hasComma = true;
                }
            }
            else if ((vchar >= 48) && (vchar <= 57)) // numeri
            {
                valid = true;
                hasNumbers = true;
            }
            else // altri caratteri
            {
                valid = false;
            }

            return valid;
        }

        internal string ParseInputString(string str)
        {
            StringBuilder buffer = new StringBuilder();
            if (str != null)
            {

                bool hasComma = false;
                bool hasNumbers = false;
                for (int i = 0; i < str.Length; i++)
                {
                    if (ValidateChar(str[i], ref hasComma, ref hasNumbers))
                    {
                        buffer.Append(str[i]);
                    }
                }

            }
            return buffer.ToString();
        }
        
        public int DecimalPlace
        {
            get
            {
                return _decimalPlace;
            }
            set
            {
                if (value >= 0)
                {
                    _decimalPlace = value;
                    _onError = !this.Validate();
                    this.Invalidate();
                }
            }
        }
        public LightCollection<NumericRange> Ranges
        {
            get
            {
                return _ranges;
            }
        }
        public decimal Value
        {
            get
            {
                return _value;
            }
        }
        public bool AllowError
        {
            get
            {
                return _allowError;
            }
            set
            {
                _allowError = value;
            }
        }
        public override string Text
        {
            get
            {
                return _numBox.Text;
            }
            set
            {
                _numBox.Text = value;
            }
        }

        public override void Refresh()
        {
            //base.Refresh();

            _onError = !Validate();
            this.Invalidate();
        }
    }
}

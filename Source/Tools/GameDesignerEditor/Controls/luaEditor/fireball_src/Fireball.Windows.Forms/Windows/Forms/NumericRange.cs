using System;
using System.Collections.Generic;
using System.Text;

namespace Fireball.Windows.Forms
{
    public class NumericRange
    {
        private decimal _minValue;
        private decimal _maxValue;
        private bool _external;

        public NumericRange()
        {
            _minValue = 0;
            _maxValue = 100;
            _external = false;
        }
        public NumericRange(decimal minValue,decimal maxValue)
        {
            _minValue = minValue;
            _maxValue = maxValue;
            _external = false;
        }

        public decimal MinValue
        {
            get { return _minValue; }
            set { _minValue = value; }
        }
        public decimal MaxValue
        {
            get { return _maxValue; }
            set { _maxValue = value; }
        }

        public bool Validate(decimal value)
        {
            if (_external)
            {
                return !((value >= _minValue) && (value <= _maxValue)); //((value <= _minValue) || (value >= _maxValue));
            }
            return ((value >= _minValue) && (value <= _maxValue));
        }

        public bool External
        {
            get
            {
                return _external;
            }
            set
            {
                _external = value;
            }
        }

        public override string ToString()
        {
            return "minValue: " + _minValue.ToString() + ", maxValue: " + _maxValue.ToString();
        }
    }
}

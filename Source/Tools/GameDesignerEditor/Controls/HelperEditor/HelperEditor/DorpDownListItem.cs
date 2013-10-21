using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HelperEditor
{
    [Serializable]
    public class DorpDownListItem
    {
        private string _key = string.Empty;
        private string _value = string.Empty;
        private int _index = 0;
        private string _text;

        public DorpDownListItem(string pKey, string pValue, int pIndex)
        {
            _key = pKey;
            _value = pValue;
            _index = pIndex;
            _text = null;
        }

        public DorpDownListItem(string[] tData, int pIndex)
        {
            _index = pIndex;

            if (tData.Length < 1)
            {
                _key = "";
                _value = "";
            }
            else if (tData.Length < 2)
            {
                if (tData[0] != null)
                {
                    _key = tData[0];
                }
                else
                {
                    _key = "";
                }
                _value = "";
            }
            else
            {
                if (tData[0] != null)
                {
                    _key = tData[0];
                }
                else
                {
                    _key = "";
                }
                if (tData[1] != null)
                {
                    _value = tData[1];
                }
                else
                {
                    _value = "";
                }
            }
        }

        public override string ToString()
        {
            return this._value;
        }
        public string Key
        {
            get
            {
                return this._key;
            }
            set
            {
                this._key = value;
            }
        }
        public string Value
        {
            get
            {
                return this._value;
            }
            set
            {
                this._value = value;
            }
        }

        public int Index
        {
            get
            {
                return this._index;
            }
            set
            {
                this._index = value;
            }
        }

        public string Text
        {
            get
            {
                return this._text;
            }
            set
            {
                this._text = value;
            }
        }
    }
}

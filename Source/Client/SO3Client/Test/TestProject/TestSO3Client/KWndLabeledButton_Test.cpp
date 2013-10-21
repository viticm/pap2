#include "stdafx.h"
#ifdef _UNIT_TEST
#include "KWndLabeledButton_Test.h"
#include "../ui/elem/wndlabeledbutton.h"

CPPUNIT_TEST_SUITE_REGISTRATION(KWndLabeledButton_Test);

void KWndLabeledButton_Test::setUp()
{
}

void KWndLabeledButton_Test::tearDown()
{
}

void KWndLabeledButton_Test::testClone()
{
	UI::KWndLabeledButton uiKWndLabeledButton;
	UI::KWndLabeledButton* puiKWndLabeledButton = new UI::KWndLabeledButton;
	uiKWndLabeledButton.Clone(puiKWndLabeledButton);
}
#endif

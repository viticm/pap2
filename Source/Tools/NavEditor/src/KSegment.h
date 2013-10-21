#ifndef _KSEGMENT_H_
#define _KSEGMENT_H_

class KSegment
{
public:
    KSegment();

    // Cfg : Configuration of the vertices
    // Values:
    //         20 IN,
    //         1X Intersected edge X{1,2,3} of the face,
    //         0X Coincident vertice X{1,2,3} of the face,
    //         0 otherwise
    unsigned int        m_cfg1;
    unsigned int        m_cfg2;

    // if cfgX >0, vX is the vertice index of the face
    int                 m_v1;
    int                 m_v2;
};

#endif  // _KSEGMENT_H_

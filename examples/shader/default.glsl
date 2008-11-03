uniform sampler2D sampler0;

void main(void)
{
        vec4 sample;
        sample = texture2D(sampler0,
        gl_TexCoord[0].st);
        gl_FragColor = 1-sample;
	gl_FragColor.a = gl_Color.a;
}


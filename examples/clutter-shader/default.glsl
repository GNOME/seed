uniform float radius = 50;
uniform sampler2D rectTexture;
uniform float x_step = 0.0001, y_step = 0.0001;
        
void main()
{
       vec4 color = texture2D(rectTexture, gl_TexCoord[0].st);
       float u;
       float v;
       int count = 1;
       for (u=-radius;u<radius;u++)
         for (v=-radius;v<radius;v++)
           {
             color += texture2D(rectTexture, 
                                vec2(gl_TexCoord[0].s + u
                                     * 2.0 * x_step,
                                     gl_TexCoord[0].t + v
                                     * 2.0 * y_step));
             count ++;
           }
   
       gl_FragColor = color / float(count);
       gl_FragColor = gl_FragColor * gl_Color;
}

<Shader>
	<Properties>
		<Property type="Texture" name="Scene"/>
	</Properties>
	
	<RenderState>
		<BlendState>Opaque</BlendState>
		<DepthState>DepthNone</DepthState>
		<RasterState>CullNone</RasterState>
	</RenderState>
	
	<Attributes>
		<Attribute name="POSITION" index="0" slotClass="PER_VERTEX" stepRate="0"/>
		<Attribute name="TEXCOORD" index="0" slotClass="PER_VERTEX" stepRate="0"/>
	</Attributes>
	
	<Shaders>
		<ShaderPath type="VS" source="Assets/Shaders/PostProcess/blit.hlsl" entry="vert"/>
		<ShaderPath type="FS" source="Assets/Shaders/PostProcess/blit.hlsl" entry="frag"/>
	</Shaders>
</Shader>
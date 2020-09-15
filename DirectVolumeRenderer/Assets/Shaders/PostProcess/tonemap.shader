<Shader>
	<Properties>
		<Property type="Texture" name="Scene"/>
		<Property type="Float" name="_Gamma"/>
		<Property type="Float" name="_Exposure"/>
		<Property type="Int" name="_ToneMapType"/>
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
		<ShaderPath type="VS" source="Assets/Shaders/PostProcess/tonemap.hlsl" entry="vert"/>
		<ShaderPath type="FS" source="Assets/Shaders/PostProcess/tonemap.hlsl" entry="frag"/>
	</Shaders>
</Shader>
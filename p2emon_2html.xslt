<?xml version="1.0" encoding="utf-8"?>
<xsl:transform version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="https://www.github.com/dwugofsk/p2emon/html">

<xsl:variable name="uppercase" value='ABCDEFGHIJKLMNOPQRSTUVWXYZÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖØÙÚÛÜÝÞŸŽŠŒ'/>
<xsl:variable name="lowercase" value='abcdefghijklmnopqrstuvwxyzàáâãäåæçèéêëìíîïðñòóôõöøùúûüýþÿžšœ'/>
<!-- Need to make perception-base-modifier -->
<!-- Need skills, resistances, and weaknesses to have name, base, and modifier -->
<!-- Make the (base, modifier, traits) group be a "quantity" element: abilities, saves, ac, hp -->
<!-- Not everything has a land speed. Speed will be (name, amount[, units]) with default units to parent node -->
<!-- Feat descriptions should be optional (for AOO) -->

<xsl:template match="//p2emon">
	<div class="p2emon">
		<xsl:attribute name="id">
			<xsl:value-of select=".@name"/>
		</xsl:attribute>
		<div class="header">
			<div class="name"><xsl:value-of select="./name"/></div>
			<div class="level"><xsl:value-of select="./level"/></div>
		</div>
		<div class="traits">
			<xsl:choose>
				<xsl:when test="./rarity = 'uncommon'"><div class="trait rarity">Unommon</div></xsl:when>
				<xsl:when test="./rarity = 'rare'"><div class="trait rarity">Rare</div></xsl:when>
				<xsl:when test="./rarity = 'legendary'"><div class="trait rarity">Legendary</div></xsl:when>
				<xsl:when test="./rarity = 'unique'"><div class="trait rarity">Unique</div></xsl:when>
			</xsl:choose>
			<xsl:choose>
				<xsl:when test="./alignment = 'lg'"><div class="trait alignment">LG</div></xsl:when>
				<xsl:when test="./alignment = 'ng'"><div class="trait alignment">NG</div></xsl:when>
				<xsl:when test="./alignment = 'cg'"><div class="trait alignment">CG</div></xsl:when>
				<xsl:when test="./alignment = 'ln'"><div class="trait alignment">LN</div></xsl:when>
				<xsl:when test="./alignment = 'n'"><div class="trait alignment">N</div></xsl:when>
				<xsl:when test="./alignment = 'cn'"><div class="trait alignment">CN</div></xsl:when>
				<xsl:when test="./alignment = 'le'"><div class="trait alignment">LE</div></xsl:when>
				<xsl:when test="./alignment = 'ne'"><div class="trait alignment">NE</div></xsl:when>
				<xsl:when test="./alignment = 'ce'"><div class="trait alignment">CE</div></xsl:when>
			</xsl:choose>
			<xsl:choose>
				<xsl:when test="./size = 'tiny'"><div class="trait size">Tiny</div></xsl:when>
				<xsl:when test="./size = 'small'"><div class="trait size">Small</div></xsl:when>
				<xsl:when test="./size = 'medium'"><div class="trait size">Medium</div></xsl:when>
				<xsl:when test="./size = 'large'"><div class="trait size">Large</div></xsl:when>
				<xsl:when test="./size = 'huge'"><div class="trait size">Huge</div></xsl:when>
				<xsl:when test="./size = 'gargantuan'"><div class="trait size">Gargantuan</div></xsl:when>
			</xsl:choose>
			<xsl:apply-templates select="./traits" mode="tag"/>
			<div class="trait type"><xsl:value-of select="./type"/></div>
		</div>
		<div class="upper">
			<p>
				<h3>Perception</h3>
				<xsl:apply-templates select="./perception" mode="quantity"/>
				<xsl:if test="count(./senses/sense) &gt; 0">
					<xsl:text>; </xsl:text>
					<xsl:for-each select="./senses/sense">
						<xsl:if test="position() &gt; 1"><xsl:text>, </xsl:text></xsl:if>
						<xsl:value-of select="node()"/>
					</xsl:for-each>
				</xsl:if>
			</p>
			<xsl:if test="count(./skills/skill) &gt; 0">
				<p>
					<h3>Skills</h3>
					<xsl:for-each select="./skills/skill">
						<xsl:if test="position() &gt; 1"><xsl:text>, </xsl:text></xsl:if>
						<xsl:value-of select="./name"/>
						<xsl:text> </xsl:text>
						<xsl:apply-templates select="./quantity"/>
					</xsl:for-each>
				</p>
			</xsl:if>
			<p>
				<h3>STR</h3><xsl:apply-templates select="abilities/strength" mode="quantity"/><xsl:text>, </xsl:text>
				<h3>DEX</h3><xsl:apply-templates select="abilities/dexterity" mode="quantity"/><xsl:text>, </xsl:text>
				<h3>CON</h3><xsl:apply-templates select="abilities/constitution" mode="quantity"/><xsl:text>, </xsl:text>
				<h3>INT</h3><xsl:apply-templates select="abilities/intelligence" mode="quantity"/><xsl:text>, </xsl:text>
				<h3>WIS</h3><xsl:apply-templates select="abilities/wisdom" mode="quantity"/><xsl:text>, </xsl:text>
				<h3>CHA</h3><xsl:apply-templates select="abilities/charisma" mode="quantity"/>
			</p>
			<xsl:for-each select="./interactions/interaction">
				<p>
					<h3><xsl:value-of select="./name"/></h3>
					<xsl:apply-templates select="description"/>
				</p>
			</xsl:for-each>
		</div>
		<div class="middle">
			<p>
				<h3>AC</h3><xsl:apply-templates select="armors/armor" mode="quantity"/>
				<xsl:text>; </xsl:text>
				<h3>Fort</h3><xsl:apply-templates select="saves/fortitude" mode="quantity"/>
				<xsl:text>, </xsl:text>
				<h3>Ref</h3><xsl:apply-templates select="saves/reflex" mode="quantity"/>
				<xsl:text>, </xsl:text>
				<h3>Will</h3><xsl:apply-templates select="saves/will" mode="quantity"/>
				<xsl:if test="count(./saves/modifier) &gt; 0">
					<xsl:text> (</xsl:text><xsl:value-of select="./saves/modifier"/><xsl:text>)</xsl:text>
				</xsl:if>
			</p>
			<p>
				<h3>HP</h3><xsl:apply-templates select="hitpoints" mode="quantity"/>
				<xsl:if test="count(./immunities/immunity) &gt; 0">
					<xsl:text>; </xsl:text>
					<h3>Immunities</h3><xsl:apply-templates select="immunities" mode="list_csv"/><xsl:text>; </xsl:text>
				</xsl:if>
				<xsl:if test="count(./weaknesses/weakeness) &gt; 0">
					<xsl:text>; </xsl:text>
					<h3>Weaknesses</h3><xsl:apply-templates select="weaknesses" mode="list_quantity"/><xsl:text>; </xsl:text>
				</xsl:if>
				<xsl:if test="count(./resistances/resistance) &gt; 0">
					<xsl:text>; </xsl:text>
					<h3>Resistances</h3><xsl:apply-templates select="resistances" mode="list_quantity"/>
				</xsl:if>
			</p>
			<xsl:apply-templates select="offturn/feat"/>
		</div>
		<div class="lower">
			<xsl:if "count(./speeds/speed) != 0">
				<p>
					<h3>Speed</h3>
					<xsl:for-each select="./speeds/speed">
						<xsl:if test="./name != 'land'"><xsl:value-of select="./name"/><xsl:text> </xsl:text></xsl:if>
						<xsl:value-of select="./amount"/><xsl:text> </xsl:text>
						<xsl:if test="count(./units) != 0"><xsl:value-of select="./units"/><xsl:text> </xsl:text></xsl:if>
						<xsl:if test="count(./units) = 0"><xsl:value-of select="../units"/><xsl:text> </xsl:text></xsl:if>
					</xsl:for-each>
				</p>
			</xsl:if>
			<xsl:for-each select="./actions/action">
				<p>
					<h3><xsl:value-of select="./type"/></h3>
					<xsl:apply-templates select="count"/>
					<xsl:value-of select="./name"/><xsl:text> </xsl:text>
					<xsl:apply-templates select="attack" mode="quantity"/>
					<xsl:if test="count(./traits) &gt; 0">
						<xsl:text> (</xsl:text><xsl:apply-templates select="traits" mode="list_csv"/><xsl:text>)</xsl:text>
					</xsl:if>
					<xsl:text>, </xsl:text>
					<xsl:if test="count(./results/damage) != 0">
						<h3>Damage</h3>
					</xsl:if>
					<xsl:if test="count(./results/damage) = 0">
						<h3>Effect</h3>
					</xsl:if>
					<xsl:for-each select="./results/damage | ./results/effect">
						<!--
							Damage: 1d4+5 fire
							Damage: 1d4+5 fire plus 1d8 cold
							Damage: 1d4+5 fire plus 1d8 cold and 1d12 lightning
							Damage: 1d4+5 fire plus 1d8 cold, 1d12 lightning, and Grab
						-->
						<xsl:if test="position() = 2"><xsl:text> plus </xsl:text>
						<xsl:if test="position() > 2 and count(../damage | ../effect) > 3">
							<xsl:text>,</xsl:text>
							<xsl:if test="position() != last()"><xsl:text> </xsl:text>
						</xsl:if>
						<xsl:if test="position() = last() and count(../damage | ../effect) > 2"> and </xsl:if>
						<xsl:if test="local-name() = 'damage'">
							<xsl:value-of select="./dice"/><xsl:text> </xsl:text><xsl:value-of select="type"/>
						</xsl:if>
						<xsl:if test="local-name() != 'damage'">
							<xsl:value-of select="."/>
						</xsl:if>
					</xsl:for-each>
				</p>
			</xsl:for-each>
			<!-- spells -->
			<xsl:if test="count(./spells/spell) != 0">
			</xsl:if>
			<xsl:apply-templates select="onturn/feat"/>
		</div>
	</div>
</xsl:template>

<xsl:template match="traits" mode="tag">
	<xsl:for-each "./trait">
		<div class="trait"><xsl:value-of "."></xsl:value-of>
	</xsl:for-each>
</xsl:template>

<xsl:template match="*" mode="modifier_num">
	<xsl:value-of select="format-number(., '+#,#')"/>
</xsl:template>

<xsl:template match="*" mode="quantity">
	<xsl:apply-templates select="./base" mode="modifier_num"/>
	<xsl:if test="count(./modifier) &gt; 0">
		<xsl:text> (</xsl:text><xsl:value-of select="./modifier"/><xsl:text>)</xsl:text>
	</xsl:if>
	<xsl:if test="count(./traits/trait) &gt; 0">
		<xsl:text>, </xsl:text>
		<xsl:apply-templates select="traits" mode="list_space"/>
	</xsl:if>
</xsl:template>

<xsl:template match="*" mode="list_csv">
	<xsl:for-each select="./*">
		<xsl:if test="position() &gt; 1"><xsl:text>, </xsl:text></xsl:if>
		<xsl:value-of select="."/>
	</xsl:for-each>
</xsl:template>

<xsl:template match="*" mode="list_space">
	<xsl:for-each select="./*">
		<xsl:if test="position() &gt; 1"><xsl:text> </xsl:text></xsl:if>
		<xsl:value-of select="."/>
	</xsl:for-each>
</xsl:template>

<xsl:template match="*" mode="list_quantity">
	<xsl:for-each select="./*">
		<xsl:if test="position() &gt; 1"><xsl:text>, </xsl:text></xsl:if>
		<xsl:if test="count(name) != 0">
			<xsl:value-of select="name"/>
		</xsl:if>
		<xsl:apply-templates select="." mode="quantity"/>
	</xsl:for-each>
</xsl:template>

<xsl:template match="count">
	<xsl:choose>
		<xsl:when test=". = 'free'"><div class="action_icon free"></div></xsl:when>
		<xsl:when test=". = 'reaction'"><div class="action_icon reaction"></div></xsl:when>
		<xsl:when test=". = 1"><div class="action_icon single"></div></xsl:when>
		<xsl:when test=". = 2"><div class="action_icon double"></div></xsl:when>
		<xsl:when test=". = 3"><div class="action_icon triple"></div></xsl:when>
	</xsl:choose>
</xsl:template>

<xsl:template mode="enumeration">
	<xsl:choose>
		<xsl:when test="string(number(.)) = 'NaN'"><xsl:value-of select="."/></xsl:when>
		<xsl:when test=". = 1"><xs:text>1st</xs:text></xsl:when>
		<xsl:when test=". = 2"><xs:text>2nd</xs:text></xsl:when>
		<xsl:when test=". = 3"><xs:text>3rd</xs:text></xsl:when>
		<xsl:when test=". &gt;= 4"><xsl:value-of select="."/><xs:text>th</xs:text></xsl:when>
	</xsl:choose>
</xsl:template>

<xsl:template match="feat">
	<p>
		<h3><xsl:value-of select="./name"/></h3>
		<xsl:apply-templates select="count"/>
		<xsl:if test="count(./traits/trait) != 0">
			<xsl:text>(</xsl:text><xsl:apply-templates select="traits" mode="list_csv"/><xsl:text>) </xsl:text>
		</xsl:if>
		<xsl:apply-templates select="./description"/>
	</p>
</xsl:template>

<xsl:template match="description">
	<span class="desc">
		<xsl:copy-of select="node() | @*"/>
	</span>
</xsl:template>

<xsl:template match="slots">
	<xsl:text>(x</xsl:text><xsl:value-of select="."/><xsl:text>)</xsl:text>
</xsl:template>

<xsl:template match="spell">
	<p>
		<h3>
			<xsl:choose>
				<xsl:when test="./tradition = 'arcane'">Arcane</xsl:when>
				<xsl:when test="./tradition = 'divine'">Divine</xsl:when>
				<xsl:when test="./tradition = 'occult'">Occult</xsl:when>
				<xsl:when test="./tradition = 'primal'">Primal</xsl:when>
			</xsl:choose>
			<xsl:text> </xsl:text>
			<xsl:choose>
				<xsl:when test="./type = 'prepared'">Prepared</xsl:when>
				<xsl:when test="./type = 'spontaneous'">Spontaneous</xsl:when>
				<xsl:when test="./type = 'innate'">Innate</xsl:when>
				<xsl:when test="./type = 'focus'">Focus</xsl:when>
				<xsl:when test="./type = 'ritual'">Ritual</xsl:when>
			</xsl:choose>
			<xsl:text> </xsl:text>
			<xsl:choose>
				<xsl:when test="count(.//spell) &gt; 1">
					Spells
				</xsl:when>
				<xsl:otherwise>
					Spell
				</xsl:otherwise>
			</xsl:choose>
		</h3>
		<xsl:text>DC </xsl:text><xsl:value-of select="./difficulty"/>
		<xsl:if test="count(./modifier) != 0">
			<xsl:text>, attack </xsl:text><xsl:apply-templates select="modifier" mode="quantity"/>
		</xsl:if>
		<xsl:text>; </xsl:text>
		<xsl:for-each "./group">
			<h3>
				<xsl:apply-templates select="level" mode="enumeration"/>
				<xsl:if test="count(./height) != 0">
					<xsl:text> (</xsl:text>
					<xsl:apply-templates select="height" mode="enumeration"/>
					<xsl:text>)</xsl:text>
				</xsl:if>
			</h3>
			<xsl:if test="count(./slots) != 0">
				<xsl:apply-templates select="slots"/>
				<xsl:text> </xsl:text>
			</xsl:if>
			<xsl:for-each select="./spell">
				<xsl:if test="position() != 1"><xsl:text> </xsl:text></xsl:if>
				<i><xsl:value-of select="./name"/></i>
				<xsl:if test="count(./slots) != 0"><xsl:text> </xsl:text><xsl:apply-templates select="slots"/></xsl:if>
			</xsl:for-each>
			<xsl:if test="position() != last()"><xsl:text>; </xsl:text></xsl:if>
		</xsl:for-each>
	</p>
</xsl:template>

<xsl:template match="*" mode="camelcase">
	<xsl:param name="text"/>
	<xsl:choose>
		<xsl:when test="contains($text, ' ')">
			<xsl:call-template name="capitalize">
				<xsl:with-param name="text" select="substring-before($text, ' ')"/>
			</xsl:call-template>
			<xsl:text> </xsl:text>
			<xsl:call-template name="camelcase">
				<xsl:with-param name="text" select="substring-after($text, ' ')"/>
			</xsl:call-template>
		</xsl:when>
		<xsl:otherwise>
			<xsl:call-template name="capitalize">
				<xsl:with-param name="text" select="$text"/>
			</xsl:call-template>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>

<xsl:template match="*" mode="capitalize">
	<xsl:param name="text"/>
	<xsl:value-of select="transform(substring($text,1,1),$lowercase,$uppercase)"/>
	<xsl:value-of select="transform(substring($text,2,string-length($text)-1),$uppercase,$lowercase)"/>
</xsl:template>

</xsl:transform>
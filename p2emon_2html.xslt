<?xml version="1.0" encoding="utf-8"?>
<xsl:transform version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="https://www.github.com/dwugofsk/p2emon/html">

<!-- 
	============================================================================
	 Capitalization
	============================================================================
-->
<!-- variables to keep track of uppercase and lowercase numbers -->
<xsl:variable name="uppercase" value='ABCDEFGHIJKLMNOPQRSTUVWXYZÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖØÙÚÛÜÝÞŸŽŠŒ'/>
<xsl:variable name="lowercase" value='abcdefghijklmnopqrstuvwxyzàáâãäåæçèéêëìíîïðñòóôõöøùúûüýþÿžšœ'/>

<!-- Capitalize the starting letter of each word in a block of text, and all other letters lowercase -->
<xsl:template name="camelcase">
	<xsl:param name="text"/>
	<xsl:choose>
		<xsl:when test="contains($text, ' ')"><!-- When the text has multiple words -->
			<!-- Capitalize the first word -->
			<xsl:call-template name="capitalize">
				<xsl:with-param name="text" select="substring-before($text, ' ')"/>
			</xsl:call-template>
			<!-- Add a space -->
			<xsl:text> </xsl:text>
			<!-- And then make the rest of the word lowercase -->
			<xsl:call-template name="camelcase">
				<xsl:with-param name="text" select="substring-after($text, ' ')"/>
			</xsl:call-template>
		</xsl:when>
		<xsl:otherwise><!-- When there is only one word, capitalize it -->
			<xsl:call-template name="capitalize">
				<xsl:with-param name="text" select="$text"/>
			</xsl:call-template>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>

<!-- Capitalize the starting letter of a block of text, make everything else lowercase -->
<xsl:template name="capitalize">
	<xsl:param name="text"/>
	<xsl:value-of select="transform(substring($text,1,1),$lowercase,$uppercase)"/>
	<xsl:value-of select="transform(substring($text,2,string-length($text)-1),$uppercase,$lowercase)"/>
</xsl:template>

<!-- 
	============================================================================
	 Main block
	============================================================================
-->
<!-- The main template: match each p2emon element -->
<xsl:template match="//p2emon">
	<div class="p2emon">
		<!-- Set the "id" attribute of the p2emon block to the "id" attribute of the p2emon element -->
		<xsl:attribute name="id">
			<xsl:value-of select=".@id"/>
		</xsl:attribute>
		<!-- The header, containing the name and level of the creature -->
		<div class="header">
			<div class="name"><xsl:value-of select="./name"/></div>
			<div class="level">Creature <xsl:value-of select="./level"/></div>
		</div>
		<!-- Display the traits in boxes below the header -->
		<div class="traits">
			<!-- Need to translate between rarity levels. If the rarity is common, then we can ignore it -->
			<xsl:choose>
				<xsl:when test="./rarity = 'uncommon'"><div class="trait rarity">Unommon</div></xsl:when>
				<xsl:when test="./rarity = 'rare'"><div class="trait rarity">Rare</div></xsl:when>
				<xsl:when test="./rarity = 'legendary'"><div class="trait rarity">Legendary</div></xsl:when>
				<xsl:when test="./rarity = 'unique'"><div class="trait rarity">Unique</div></xsl:when>
			</xsl:choose>
			<!-- Need to translate alignment types -->
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
			<!-- Need to translate sizes -->
			<xsl:choose>
				<xsl:when test="./size = 'tiny'"><div class="trait size">Tiny</div></xsl:when>
				<xsl:when test="./size = 'small'"><div class="trait size">Small</div></xsl:when>
				<xsl:when test="./size = 'medium'"><div class="trait size">Medium</div></xsl:when>
				<xsl:when test="./size = 'large'"><div class="trait size">Large</div></xsl:when>
				<xsl:when test="./size = 'huge'"><div class="trait size">Huge</div></xsl:when>
				<xsl:when test="./size = 'gargantuan'"><div class="trait size">Gargantuan</div></xsl:when>
			</xsl:choose>
			<!-- Special creature tags are listed here -->
			<xsl:apply-templates select="traits" mode="tag"/>
			<!-- Items are shown here -->
			<xsl:if test="count(./items/item) != 0">
				<p>
					<h3>Items </h3>
					<xsl:apply-templates select="items/item"/>
				</p>
			</xsl:for-each>
			<!-- Lastly, the type of the creature (e.g. undead) is listed here -->
			<div class="trait type"><xsl:value-of select="./type"/></div>
		</div>
		<!-- Specify the upper portion of the creature block -->
		<div class="upper">
			<!-- Perception can be of the form
				Perception +18 (+23 in special circumstances); darkvision 120 feet, blindsight 30 feet
			-->
			<p>
				<h3>Perception </h3>
				<xsl:apply-templates select="perception" mode="quantity"/>
			</p>
			<!-- Skills are of the following form
				Skills Athletics +4 (+5 in special cases), Stealth +3, ...
			-->
			<xsl:if test="count(./skills/skill) &gt; 0">
				<p>
					<h3>Skills </h3>
					<xsl:for-each select="./skills/skill">
						<xsl:if test="position() &gt; 1"><xsl:text>, </xsl:text></xsl:if>
						<xsl:value-of select="./name"/>
						<xsl:text> </xsl:text>
						<xsl:apply-templates select="./quantity"/>
					</xsl:for-each>
				</p>
			</xsl:if>
			<!--
				The ability modifiers are listed together

				Ability modifiers are presented in the following format
				STR +4 (+5 in special cases), DEX +3, CON +2, ...
			-->
			<p>
				<h3>STR </h3><xsl:apply-templates select="abilities/strength" mode="quantity"/><xsl:text>, </xsl:text>
				<h3>DEX </h3><xsl:apply-templates select="abilities/dexterity" mode="quantity"/><xsl:text>, </xsl:text>
				<h3>CON </h3><xsl:apply-templates select="abilities/constitution" mode="quantity"/><xsl:text>, </xsl:text>
				<h3>INT </h3><xsl:apply-templates select="abilities/intelligence" mode="quantity"/><xsl:text>, </xsl:text>
				<h3>WIS </h3><xsl:apply-templates select="abilities/wisdom" mode="quantity"/><xsl:text>, </xsl:text>
				<h3>CHA </h3><xsl:apply-templates select="abilities/charisma" mode="quantity"/>
			</p>
			<!--
				Interactions cover details about how the creature interacts with the world (e.g. becoming broken, being used a certain way) that are typically not relevant in combat

				Interactions are of the form
				[Interaction Name] [Description of interactions]
			-->
			<xsl:apply-templates select="interactions/feat"/>
		</div>
		<!-- 
			The middle section covers AC, saves, hitpoints, immunities, weaknesses, resistances, and features that tend to proc off-turn
		-->
		<div class="middle">
			<!--
				AC and saves are listed together, as well as universal modifiers to saves
				
				AC is of the form:
				AC 14 (18 with mage armor), all-around vision

				Saves are of the form:
				[Fort|Ref|Will] +4 (+8 in special cases)

				Unversial modifier to saves are displayed
				(+1 to all saves against magic)
			-->
			<p>
				<h3>AC </h3><xsl:apply-templates select="armors/armor" mode="quantity_flat"/>
				<xsl:text>; </xsl:text>
				<h3>Fort </h3><xsl:apply-templates select="saves/fortitude" mode="quantity"/>
				<xsl:text>, </xsl:text>
				<h3>Ref </h3><xsl:apply-templates select="saves/reflex" mode="quantity"/>
				<xsl:text>, </xsl:text>
				<h3>Will </h3><xsl:apply-templates select="saves/will" mode="quantity"/>
				<xsl:if test="count(./saves/modifier) &gt; 0">
					<xsl:text> (</xsl:text><xsl:value-of select="./saves/modifier"/><xsl:text>)</xsl:text>
				</xsl:if>
			</p>
			<!--
				HP, immunities, weaknesses, and saves are all displayed together

				Hitpoints are displayed in the following format:
				HP 300 (184 in special cases), negative healing

				Immunities are displayed in the following format:
				Immunities fire, cold, electricity, ...

				Weaknesses and Resistances are displayed in the following format:
				[Weaknesses|Resistances] fire 10, cold 13 (15 in special cases)
			-->
			<p>
				<h3>HP </h3><xsl:apply-templates select="hitpoints" mode="quantity"/>
				<xsl:if test="count(./immunities/immunity) &gt; 0">
					<xsl:text>; </xsl:text>
					<h3>Immunities </h3><xsl:apply-templates select="immunities" mode="list_csv"/><xsl:text>; </xsl:text>
				</xsl:if>
				<xsl:if test="count(./weaknesses/weakeness) &gt; 0">
					<xsl:text>; </xsl:text>
					<h3>Weaknesses </h3><xsl:apply-templates select="weaknesses" mode="list_quantity"/><xsl:text>; </xsl:text>
				</xsl:if>
				<xsl:if test="count(./resistances/resistance) &gt; 0">
					<xsl:text>; </xsl:text>
					<h3>Resistances </h3><xsl:apply-templates select="resistances" mode="list_quantity_flat"/>
				</xsl:if>
			</p>
			<!--
				List feats here that are mostly relevant on other creatures' turns
			-->
			<xsl:apply-templates select="offturn/feat"/>
		</div>
		<!-- The lower section covers speed, actions, spells, and feats relevant on a creature's turn -->
		<div class="lower">
			<!--
				If the creature has some form of speed definition (which they always should)
				Display the speed, including non-land speeds

				The speed spec is displayed as follows:
				Speed 40 ft; flying 120 ft; climb 30 ft; ...
			-->
			<xsl:if "count(./speeds/speed) != 0">
				<p>
					<h3>Speed </h3>
					<xsl:for-each select="./speeds/speed">
						<xsl:if test="./name != 'land'"><xsl:value-of select="./name"/><xsl:text> </xsl:text></xsl:if>
						<xsl:value-of select="./amount"/><xsl:text> </xsl:text>
						<xsl:if test="count(./units) != 0"><xsl:value-of select="./units"/><xsl:text> </xsl:text></xsl:if>
						<xsl:if test="count(./units) = 0"><xsl:value-of select="../units"/><xsl:text> </xsl:text></xsl:if>
					</xsl:for-each>
				</p>
			</xsl:if>
			<!--
				Display the actions

				Actions are displayed as follows:
				[Melee | Ranged] action name [>|>>|>>>|-|<-] +18 (trait1, trait2, ...), [Damage | Effect] [Damage description]
			-->
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
						<h3>Damage </h3>
					</xsl:if>
					<xsl:if test="count(./results/damage) = 0">
						<h3>Effect </h3>
					</xsl:if>
					<xsl:for-each select="./results/damage | ./results/effect">
						<!--
							Damage: 1d4+5 fire
							Damage: 1d4+5 fire plus 1d8 cold
							Damage: 1d4+5 fire plus 1d8 cold and 1d12 lightning
							Damage: 1d4+5 fire plus 1d8 cold, 1d12 lightning, and Grab

							or

							Effect: Special Effect
							Effect: Special Effect plus Grab
							Effect: Special Effect plus Grab and Knockdown
							Effect: Special Effect plus Grab, Knockdown, and Other Effect
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
			<!--
				Display spell repertories
			-->
			<xsl:apply-templates select="spells/spell"/>
			<!--
				Display feats relating to thing the creature can do on their turn
			-->
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
	<xsl:apply-templates select="base" mode="modifier_num"/>
	<xsl:if test="count(./modifier) &gt; 0">
		<xsl:text> (</xsl:text><xsl:value-of select="./modifier"/><xsl:text>)</xsl:text>
	</xsl:if>
	<xsl:if test="count(./traits/trait) &gt; 0">
		<xsl:text>, </xsl:text>
		<xsl:apply-templates select="traits" mode="list_space"/>
	</xsl:if>
</xsl:template>

<xsl:template match="*" mode="quantity_flat">
	<xsl:value-of select="./base"/>
	<xsl:if test="count(./modifier) &gt; 0">
		<xsl:text> (</xsl:text><xsl:value-of select="./modifier"/><xsl:text>)</xsl:text>
	</xsl:if>
	<xsl:if test="count(./traits/trait) &gt; 0">
		<xsl:text>, </xsl:text>
		<xsl:apply-templates select="traits" mode="list_space"/>
	</xsl:if>
</xsl:template>

<xsl:template match="*" mode="quantity_csv">
	<xsl:apply-templates select="./base" mode="modifier_num"/>
	<xsl:if test="count(./modifier) &gt; 0">
		<xsl:text> (</xsl:text><xsl:value-of select="./modifier"/><xsl:text>)</xsl:text>
	</xsl:if>
	<xsl:if test="count(./traits/trait) &gt; 0">
		<xsl:text>; </xsl:text>
		<xsl:apply-templates select="traits" mode="list_csv"/>
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

<xsl:template match="*" mode="list_quantity_flat">
	<xsl:for-each select="./*">
		<xsl:if test="position() &gt; 1"><xsl:text>, </xsl:text></xsl:if>
		<xsl:if test="count(name) != 0">
			<xsl:value-of select="name"/>
		</xsl:if>
		<xsl:apply-templates select="." mode="quantity_flat"/>
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
		<h3><xsl:value-of select="./name"/><xsl:text> </xsl:text></h3>
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

<xsl:template match="item">
	<span class="desc">
		<xsl:copy-of select="node() | @*"/>
		<xsl:if test="position() != last()">
			<xsl:text>, </xsl:text>
		</xsl:if>
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


</xsl:transform>
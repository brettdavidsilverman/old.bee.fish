<?xml version="1.0"?>
<xsl:stylesheet xmlns="http://www.w3.org/1999/xhtml" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
  <xsl:template match="javascript">
    <javascript><xsl:apply-templates select="@*|node()"/></javascript>
  </xsl:template> 
</xsl:stylesheet>
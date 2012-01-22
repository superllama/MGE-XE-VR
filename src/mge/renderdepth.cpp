
#include "proxydx/d3d8header.h"
#include "support/log.h"

#include "configuration.h"
#include "distantland.h"
#include "distantshader.h"
#include "mwbridge.h"


void DistantLand::renderDepth()
{
    DECLARE_MWBRIDGE

    // Switch to render target
    RenderTargetSwitcher rtsw(texDepthFrame, surfDepthDepth);
    device->Clear(0, 0, D3DCLEAR_ZBUFFER, 0, 1.0, 0);

    // Unbind depth sampler
    effect->SetTexture(ehTex3, NULL);

    // Projection should cover whole scene
    D3DXMATRIX distProj = mwProj;
    editProjectionZ(&distProj, 4.0f, Configuration.DL.DrawDist * 8192.0f);
    effect->SetMatrix(ehProj, &distProj);

    // Clear floating point buffer to far depth
    effectDepth->BeginPass(PASS_CLEARDEPTH);
    device->SetVertexDeclaration(WaterDecl);
    device->SetStreamSource(0, vbFullFrame, 0, 12);
    device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
    effectDepth->EndPass();

    // Recorded draw calls
    effectDepth->BeginPass(PASS_RENDERMWDEPTH);
    renderDepthRecorded();
    effectDepth->EndPass();

    if(isDistantCell())
    {
        if(!mwBridge->IsUnderwater(eyePos.z))
        {
            // Distant land
            if(mwBridge->IsExterior())
            {
                effectDepth->BeginPass(PASS_RENDERLANDDEPTH);
                renderDistantLandZ();
                effectDepth->EndPass();
            }

            // Distant statics
            effectDepth->BeginPass(PASS_RENDERSTATICSDEPTH);
            device->SetVertexDeclaration(StaticDecl);
            visDistant.Render(device, effectDepth, effect, &ehTex0, &ehHasAlpha, &ehWorld, SIZEOFSTATICVERT);
            effectDepth->EndPass();
        }

        if(Configuration.MGEFlags & USE_GRASS)
        {
            effectDepth->BeginPass(PASS_RENDERGRASSDEPTHINST);
            renderGrassInstZ();
            effectDepth->EndPass();
        }
    }

    // Reset projection matrix
    effect->SetMatrix(ehProj, &mwProj);
}

void DistantLand::renderDepthAdditional()
{
    // Switch to render target
    RenderTargetSwitcher rtsw(texDepthFrame, surfDepthDepth);

    // Unbind depth sampler
    effect->SetTexture(ehTex3, NULL);

    // Projection should cover whole scene
    D3DXMATRIX distProj = mwProj;
    editProjectionZ(&distProj, 4.0f, Configuration.DL.DrawDist * 8192.0f);
    effect->SetMatrix(ehProj, &distProj);

    // Recorded draw calls
    effectDepth->BeginPass(PASS_RENDERMWDEPTH);
    renderDepthRecorded();
    effectDepth->EndPass();

    // Reset projection matrix
    effect->SetMatrix(ehProj, &mwProj);
}

void DistantLand::renderDepthRecorded()
{
    // Recorded renders
    for(vector<RenderedState>::iterator i = recordMW.begin(); i != recordMW.end(); ++i)
    {
        // Set variables in main effect; variables are shared via effect pool
        if((i->alphaTest || i->blendEnable) && i->texture)
        {
            effect->SetTexture(ehTex0, i->texture);
            effect->SetBool(ehHasAlpha, true);
            effect->SetFloat(ehAlphaRef, i->alphaTest ? (i->alphaRef / 255.0f) : 0.5f);
        }
        else
        {
            effect->SetTexture(ehTex0, 0);
            effect->SetBool(ehHasAlpha, false);
            effect->SetFloat(ehAlphaRef, -1.0f);
        }

        effect->SetBool(ehHasBones, i->vertexBlendState != 0);
        effect->SetInt(ehVertexBlendState, i->vertexBlendState);
        effect->SetMatrixArray(ehVertexBlendPalette, i->worldTransforms, 4);
        effectDepth->CommitChanges();

        device->SetRenderState(D3DRS_CULLMODE, i->cullMode);
        device->SetStreamSource(0, i->vb, i->vbOffset, i->vbStride);
        device->SetIndices(i->ib);
        device->SetFVF(i->fvf);
        device->DrawIndexedPrimitive(i->primType, i->baseIndex, i->minIndex, i->vertCount, i->startIndex, i->primCount);
    }
}

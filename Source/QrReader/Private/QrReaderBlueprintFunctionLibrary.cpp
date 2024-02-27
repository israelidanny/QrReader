#include "QrReaderBlueprintFunctionLibrary.h"
#include "ZXingUnreal.h"

void UQrReaderBlueprintFunctionLibrary::QrScanFromTexture(UTexture2D* Texture, const FQrScanCallback& OnResult)
{
	const FTexture2DMipMap* MipMap = &Texture->GetPlatformData()->Mips[0];
	const FByteBulkData* RawImageData = &MipMap->BulkData;
	EPixelFormat Format = Texture->GetPixelFormat();
	int32_t Width = Texture->GetSizeX();
	int32_t Height = Texture->GetSizeY();
	const uint8_t* Buffer = static_cast<const uint8*>(RawImageData->LockReadOnly());

	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask,
	          [Texture, OnResult, Format, Width, Height, Buffer, RawImageData]
	          {
		          const TArray<UZXingResult*> Result =
			          UZXingBlueprintFunctionLibrary::ReadBarcodes(Texture, Format, Width, Height, Buffer);
		          AsyncTask(ENamedThreads::GameThread, [OnResult, Result, RawImageData]
		          {
			          RawImageData->Unlock();
			          if (Result.Num() > 0)
			          {
				          OnResult.ExecuteIfBound(Result);
			          }
		          });
	          });
}

void UQrReaderBlueprintFunctionLibrary::QrScanFromMediaTexture(UMediaTexture* MediaTexture,
                                                               const FQrScanCallback& OnResult)
{
	ENQUEUE_RENDER_COMMAND(CaptureMaterialToTexture)(
		[MediaTexture, OnResult](FRHICommandListImmediate& RHICmdList)
		{
			TArray<FColor> OutData;
			RHICmdList.ReadSurfaceData(
				MediaTexture->GetResource()->TextureRHI->GetTexture2D(),
				FIntRect(0, 0, MediaTexture->GetSurfaceWidth(), MediaTexture->GetSurfaceHeight()),
				OutData,
				FReadSurfaceDataFlags()
			);

			AsyncTask(ENamedThreads::GameThread, [MediaTexture, OnResult, OutData]
			{
				UTexture2D* NewTexture = UTexture2D::CreateTransient(MediaTexture->GetSurfaceWidth(),
				                                                     MediaTexture->GetSurfaceHeight());
				NewTexture->NeverStream = true;
				FTexture2DMipMap& Mip = NewTexture->GetPlatformData()->Mips[0];

				void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);
				FMemory::Memcpy(Data, OutData.GetData(), OutData.Num() * sizeof(FColor));
				Mip.BulkData.Unlock();
				NewTexture->UpdateResource();

				QrScanFromTexture(NewTexture, OnResult);
			});
		}
	);
}

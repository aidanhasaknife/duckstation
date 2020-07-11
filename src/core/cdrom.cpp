#include "cdrom.h"
#include "common/cd_image.h"
#include "common/log.h"
#include "common/state_wrapper.h"
#include "dma.h"
#include "game_list.h"
#include "imgui.h"
#include "interrupt_controller.h"
#include "settings.h"
#include "spu.h"
#include "system.h"
Log_SetChannel(CDROM);

struct CommandInfo
{
  const char* name;
  u8 expected_parameters;
};

static std::array<CommandInfo, 255> s_command_info = {{
  {"Sync", 0},
  {"Getstat", 0},
  {"Setloc", 3},
  {"Play", 0},
  {"Forward", 0},
  {"Backward", 0},
  {"ReadN", 0},
  {"MotorOn", 0},
  {"Stop", 0},
  {"Pause", 0},
  {"Reset", 0},
  {"Mute", 0},
  {"Demute", 0},
  {"Setfilter", 2},
  {"Setmode", 1},
  {"Getparam", 0},
  {"GetlocL", 0},
  {"GetlocP", 0},
  {"SetSession", 1},
  {"GetTN", 0},
  {"GetTD", 1},
  {"SeekL", 0},
  {"SeekP", 0},
  {"SetClock", 0},
  {"GetClock", 0},
  {"Test", 1},
  {"GetID", 0},
  {"ReadS", 0},
  {"Init", 0},
  {"GetQ", 2},
  {"ReadTOC", 0},
  {"VideoCD", 6},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  {"Unknown", 0},
  0 // Unknown
}};

CDROM::CDROM() = default;

CDROM::~CDROM() = default;

void CDROM::Initialize(System* system, DMA* dma, InterruptController* interrupt_controller, SPU* spu)
{
  m_system = system;
  m_dma = dma;
  m_interrupt_controller = interrupt_controller;
  m_spu = spu;
  m_command_event =
    m_system->CreateTimingEvent("CDROM Command Event", 1, 1, std::bind(&CDROM::ExecuteCommand, this), false);
  m_drive_event = m_system->CreateTimingEvent("CDROM Drive Event", 1, 1,
                                              std::bind(&CDROM::ExecuteDrive, this, std::placeholders::_2), false);

  if (m_system->GetSettings().cdrom_read_thread)
    m_reader.StartThread();
}

void CDROM::Reset()
{
  SoftReset();

  // this should be reading sector 0
  m_reader.WaitForReadToComplete();
  if (m_reader.GetSectorSubQ().IsCRCValid())
    m_last_subq = m_reader.GetSectorSubQ();
}

void CDROM::SoftReset()
{
  m_command = Command::None;
  m_command_event->Deactivate();
  m_drive_state = DriveState::Idle;
  m_drive_event->Deactivate();
  m_status.bits = 0;
  m_secondary_status.bits = 0;
  m_secondary_status.motor_on = CanReadMedia();
  m_mode.bits = 0;
  m_current_double_speed = false;
  m_interrupt_enable_register = INTERRUPT_REGISTER_MASK;
  m_interrupt_flag_register = 0;
  m_pending_async_interrupt = 0;
  m_setloc_position = {};
  m_current_lba = 0;
  if (m_reader.HasMedia())
    m_reader.QueueReadSector(m_current_lba);
  m_seek_start_lba = 0;
  m_seek_end_lba = 0;
  m_setloc_pending = false;
  m_read_after_seek = false;
  m_play_after_seek = false;
  m_muted = false;
  m_adpcm_muted = false;
  m_xa_filter_file_number = 0;
  m_xa_filter_channel_number = 0;
  m_xa_current_file_number = 0;
  m_xa_current_channel_number = 0;
  m_xa_current_set = false;
  std::memset(&m_last_sector_header, 0, sizeof(m_last_sector_header));
  std::memset(&m_last_sector_subheader, 0, sizeof(m_last_sector_subheader));
  m_last_sector_header_valid = false;
  std::memset(&m_last_subq, 0, sizeof(m_last_subq));
  m_last_cdda_report_frame_nibble = 0xFF;

  m_next_cd_audio_volume_matrix[0][0] = 0x80;
  m_next_cd_audio_volume_matrix[0][1] = 0x00;
  m_next_cd_audio_volume_matrix[1][0] = 0x00;
  m_next_cd_audio_volume_matrix[1][1] = 0x80;
  m_cd_audio_volume_matrix = m_next_cd_audio_volume_matrix;
  ResetXAResampler();

  m_param_fifo.Clear();
  m_response_fifo.Clear();
  m_async_response_fifo.Clear();
  m_data_fifo.Clear();

  m_current_read_sector_buffer = 0;
  m_current_write_sector_buffer = 0;
  for (u32 i = 0; i < NUM_SECTOR_BUFFERS; i++)
  {
    m_sector_buffers[i].data.fill(0);
    m_sector_buffers[i].size = 0;
  }

  UpdateStatusRegister();
}

bool CDROM::DoState(StateWrapper& sw)
{
  sw.Do(&m_command);
  sw.Do(&m_drive_state);
  sw.Do(&m_status.bits);
  sw.Do(&m_secondary_status.bits);
  sw.Do(&m_mode.bits);
  sw.Do(&m_current_double_speed);
  sw.Do(&m_interrupt_enable_register);
  sw.Do(&m_interrupt_flag_register);
  sw.Do(&m_pending_async_interrupt);
  sw.DoPOD(&m_setloc_position);
  sw.Do(&m_current_lba);
  sw.Do(&m_seek_start_lba);
  sw.Do(&m_seek_end_lba);
  sw.Do(&m_setloc_pending);
  sw.Do(&m_read_after_seek);
  sw.Do(&m_play_after_seek);
  sw.Do(&m_muted);
  sw.Do(&m_adpcm_muted);
  sw.Do(&m_xa_filter_file_number);
  sw.Do(&m_xa_filter_channel_number);
  sw.Do(&m_xa_current_file_number);
  sw.Do(&m_xa_current_channel_number);
  sw.Do(&m_xa_current_set);
  sw.DoBytes(&m_last_sector_header, sizeof(m_last_sector_header));
  sw.DoBytes(&m_last_sector_subheader, sizeof(m_last_sector_subheader));
  sw.Do(&m_last_sector_header_valid);
  sw.DoBytes(&m_last_subq, sizeof(m_last_subq));
  sw.Do(&m_last_cdda_report_frame_nibble);
  sw.Do(&m_play_track_number_bcd);
  sw.Do(&m_async_command_parameter);
  sw.Do(&m_cd_audio_volume_matrix);
  sw.Do(&m_next_cd_audio_volume_matrix);
  sw.Do(&m_xa_last_samples);
  sw.Do(&m_xa_resample_ring_buffer);
  sw.Do(&m_xa_resample_p);
  sw.Do(&m_xa_resample_sixstep);
  sw.Do(&m_param_fifo);
  sw.Do(&m_response_fifo);
  sw.Do(&m_async_response_fifo);
  sw.Do(&m_data_fifo);

  sw.Do(&m_current_read_sector_buffer);
  sw.Do(&m_current_write_sector_buffer);
  for (u32 i = 0; i < NUM_SECTOR_BUFFERS; i++)
  {
    sw.Do(&m_sector_buffers[i].data);
    sw.Do(&m_sector_buffers[i].size);
  }

  u32 requested_sector = (sw.IsWriting() ? (m_reader.WaitForReadToComplete(), m_reader.GetLastReadSector()) : 0);
  sw.Do(&requested_sector);

  if (sw.IsReading())
  {
    if (m_reader.HasMedia())
      m_reader.QueueReadSector(requested_sector);
    UpdateCommandEvent();
    m_drive_event->SetState(!IsDriveIdle());
  }

  return !sw.HasError();
}

void CDROM::InsertMedia(std::unique_ptr<CDImage> media)
{
  if (CanReadMedia())
    RemoveMedia();

  // set the region from the system area of the disc
  m_disc_region = GameList::GetRegionForImage(media.get());
  Log_InfoPrintf("Inserting new media, disc region: %s, console region: %s", Settings::GetDiscRegionName(m_disc_region),
                 Settings::GetConsoleRegionName(m_system->GetRegion()));

  // motor automatically spins up
  if (m_drive_state != DriveState::ShellOpening)
    m_secondary_status.motor_on = true;

  // reading TOC? interestingly this doesn't work for GetlocL though...
  CDImage::SubChannelQ subq;
  if (media->Seek(0) && media->ReadSubChannelQ(&subq) && subq.IsCRCValid())
    m_last_subq = subq;

  m_reader.SetMedia(std::move(media));
}

void CDROM::RemoveMedia(bool force /* = false */)
{
  if (!HasMedia() && !force)
    return;

  const TickCount stop_ticks = GetTicksForStop(true);

  Log_InfoPrintf("Removing CD...");
  m_reader.RemoveMedia();

  m_last_sector_header_valid = false;

  m_secondary_status.motor_on = false;
  m_secondary_status.shell_open = true;
  m_secondary_status.ClearActiveBits();
  m_disc_region = DiscRegion::Other;

  // If the drive was doing anything, we need to abort the command.
  m_drive_state = DriveState::Idle;
  m_command = Command::None;
  m_command_event->Deactivate();
  m_drive_event->Deactivate();

  // The console sends an interrupt when the shell is opened regardless of whether a command was executing.
  SendAsyncErrorResponse(STAT_ERROR, 0x08);

  // Begin spin-down timer, we can't swap the new disc in immediately for some games (e.g. Metal Gear Solid).
  if (!force)
  {
    m_drive_state = DriveState::ShellOpening;
    m_drive_event->SetIntervalAndSchedule(stop_ticks);
  }
}

void CDROM::SetUseReadThread(bool enabled)
{
  if (enabled == m_reader.IsUsingThread())
    return;

  if (enabled)
    m_reader.StartThread();
  else
    m_reader.StopThread();
}

u8 CDROM::ReadRegister(u32 offset)
{
  switch (offset)
  {
    case 0: // status register
      Log_TracePrintf("CDROM read status register -> 0x%08X", m_status.bits);
      return m_status.bits;

    case 1: // always response FIFO
    {
      if (m_response_fifo.IsEmpty())
      {
        Log_DebugPrintf("Response FIFO empty on read");
        return 0xFF;
      }

      const u8 value = m_response_fifo.Pop();
      UpdateStatusRegister();
      Log_DebugPrintf("CDROM read response FIFO -> 0x%08X", ZeroExtend32(value));
      return value;
    }

    case 2: // always data FIFO
    {
      const u8 value = m_data_fifo.Pop();
      UpdateStatusRegister();
      Log_DebugPrintf("CDROM read data FIFO -> 0x%08X", ZeroExtend32(value));
      return value;
    }

    case 3:
    {
      if (m_status.index & 1)
      {
        const u8 value = m_interrupt_flag_register | ~INTERRUPT_REGISTER_MASK;
        Log_DebugPrintf("CDROM read interrupt flag register -> 0x%02X", ZeroExtend32(value));
        return value;
      }
      else
      {
        const u8 value = m_interrupt_enable_register | ~INTERRUPT_REGISTER_MASK;
        Log_DebugPrintf("CDROM read interrupt enable register -> 0x%02X", ZeroExtend32(value));
        return value;
      }
    }
    break;
  }

  Log_ErrorPrintf("Unknown CDROM register read: offset=0x%02X, index=%d", offset,
                  ZeroExtend32(m_status.index.GetValue()));
  Panic("Unknown CDROM register");
  return 0;
}

void CDROM::WriteRegister(u32 offset, u8 value)
{
  if (offset == 0)
  {
    Log_TracePrintf("CDROM status register <- 0x%02X", value);
    m_status.bits = (m_status.bits & static_cast<u8>(~3)) | (value & u8(3));
    return;
  }

  const u32 reg = (m_status.index * 3u) + (offset - 1);
  switch (reg)
  {
    case 0:
    {
      Log_DebugPrintf("CDROM command register <- 0x%02X (%s)", value, s_command_info[value].name);
      BeginCommand(static_cast<Command>(value));
      return;
    }

    case 1:
    {
      if (m_param_fifo.IsFull())
      {
        Log_WarningPrintf("Parameter FIFO overflow");
        m_param_fifo.RemoveOne();
      }

      m_param_fifo.Push(value);
      UpdateStatusRegister();
      return;
    }

    case 2:
    {
      Log_DebugPrintf("Request register <- 0x%02X", value);
      const RequestRegister rr{value};

      // Sound map is not currently implemented, haven't found anything which uses it.
      if (rr.SMEN)
        Log_ErrorPrintf("Sound map enable set");
      if (rr.BFWR)
        Log_ErrorPrintf("Buffer write enable set");

      if (rr.BFRD)
      {
        LoadDataFIFO();
      }
      else
      {
        Log_DebugPrintf("Clearing data FIFO");
        m_data_fifo.Clear();
      }

      UpdateStatusRegister();
      return;
    }

    case 3:
    {
      Log_ErrorPrintf("Sound map data out <- 0x%02X", value);
      return;
    }

    case 4:
    {
      Log_DebugPrintf("Interrupt enable register <- 0x%02X", value);
      m_interrupt_enable_register = value & INTERRUPT_REGISTER_MASK;
      UpdateInterruptRequest();
      return;
    }

    case 5:
    {
      Log_DebugPrintf("Interrupt flag register <- 0x%02X", value);
      m_interrupt_flag_register &= ~(value & INTERRUPT_REGISTER_MASK);
      if (m_interrupt_flag_register == 0)
      {
        if (HasPendingAsyncInterrupt())
          DeliverAsyncInterrupt();
        else
          UpdateCommandEvent();
      }

      // Bit 6 clears the parameter FIFO.
      if (value & 0x40)
      {
        m_param_fifo.Clear();
        UpdateStatusRegister();
      }

      return;
    }

    case 6:
    {
      Log_ErrorPrintf("Sound map coding info <- 0x%02X", value);
      return;
    }

    case 7:
    {
      Log_DebugPrintf("Audio volume for left-to-left output <- 0x%02X", value);
      m_next_cd_audio_volume_matrix[0][0] = value;
      return;
    }

    case 8:
    {
      Log_DebugPrintf("Audio volume for left-to-right output <- 0x%02X", value);
      m_next_cd_audio_volume_matrix[0][1] = value;
      return;
    }

    case 9:
    {
      Log_DebugPrintf("Audio volume for right-to-right output <- 0x%02X", value);
      m_next_cd_audio_volume_matrix[1][1] = value;
      return;
    }

    case 10:
    {
      Log_DebugPrintf("Audio volume for right-to-left output <- 0x%02X", value);
      m_next_cd_audio_volume_matrix[1][0] = value;
      return;
    }

    case 11:
    {
      Log_DebugPrintf("Audio volume apply changes <- 0x%02X", value);
      m_adpcm_muted = ConvertToBoolUnchecked(value & u8(0x01));
      if (value & 0x20)
        m_cd_audio_volume_matrix = m_next_cd_audio_volume_matrix;
      return;
    }

    default:
    {
      Log_ErrorPrintf("Unknown CDROM register write: offset=0x%02X, index=%d, reg=%u, value=0x%02X", offset,
                      m_status.index.GetValue(), reg, value);
      return;
    }
  }
}

void CDROM::DMARead(u32* words, u32 word_count)
{
  const u32 words_in_fifo = m_data_fifo.GetSize() / 4;
  if (words_in_fifo < word_count)
  {
    Log_ErrorPrintf("DMA read on empty/near-empty data FIFO");
    std::memset(words + words_in_fifo, 0, sizeof(u32) * (word_count - words_in_fifo));
  }

  const u32 bytes_to_read = std::min<u32>(word_count * sizeof(u32), m_data_fifo.GetSize());
  m_data_fifo.PopRange(reinterpret_cast<u8*>(words), bytes_to_read);
}

void CDROM::SetInterrupt(Interrupt interrupt)
{
  m_interrupt_flag_register = static_cast<u8>(interrupt);
  UpdateInterruptRequest();
}

void CDROM::SetAsyncInterrupt(Interrupt interrupt)
{
  if (m_interrupt_flag_register == static_cast<u8>(interrupt))
  {
    Log_WarningPrintf("Not setting async interrupt %u because there is already one unacknowledged",
                      static_cast<u8>(interrupt));
    m_async_response_fifo.Clear();
    return;
  }

  Assert(m_pending_async_interrupt == 0);
  m_pending_async_interrupt = static_cast<u8>(interrupt);
  if (!HasPendingInterrupt())
    DeliverAsyncInterrupt();
}

void CDROM::ClearAsyncInterrupt()
{
  m_pending_async_interrupt = 0;
  m_async_response_fifo.Clear();
}

void CDROM::DeliverAsyncInterrupt()
{
  Assert(m_pending_async_interrupt != 0 && !HasPendingInterrupt());
  Log_DebugPrintf("Delivering async interrupt %u", m_pending_async_interrupt);

  if (m_pending_async_interrupt == static_cast<u8>(Interrupt::DataReady))
    m_current_read_sector_buffer = m_current_write_sector_buffer;

  m_response_fifo.Clear();
  m_response_fifo.PushFromQueue(&m_async_response_fifo);
  m_interrupt_flag_register = m_pending_async_interrupt;
  m_pending_async_interrupt = 0;
  UpdateInterruptRequest();
  UpdateStatusRegister();
  UpdateCommandEvent();
}

void CDROM::SendACKAndStat()
{
  m_response_fifo.Push(m_secondary_status.bits);
  SetInterrupt(Interrupt::ACK);
}

void CDROM::SendErrorResponse(u8 stat_bits /* = STAT_ERROR */, u8 reason /* = 0x80 */)
{
  m_response_fifo.Push(m_secondary_status.bits | stat_bits);
  m_response_fifo.Push(reason);
  SetInterrupt(Interrupt::Error);
}

void CDROM::SendAsyncErrorResponse(u8 stat_bits /* = STAT_ERROR */, u8 reason /* = 0x80 */)
{
  m_async_response_fifo.Push(m_secondary_status.bits | stat_bits);
  m_async_response_fifo.Push(reason);
  SetAsyncInterrupt(Interrupt::Error);
}

void CDROM::UpdateStatusRegister()
{
  m_status.ADPBUSY = false;
  m_status.PRMEMPTY = m_param_fifo.IsEmpty();
  m_status.PRMWRDY = !m_param_fifo.IsFull();
  m_status.RSLRRDY = !m_response_fifo.IsEmpty();
  m_status.DRQSTS = !m_data_fifo.IsEmpty();
  m_status.BUSYSTS = HasPendingCommand();

  m_dma->SetRequest(DMA::Channel::CDROM, m_status.DRQSTS);
}

void CDROM::UpdateInterruptRequest()
{
  if ((m_interrupt_flag_register & m_interrupt_enable_register) == 0)
    return;

  m_interrupt_controller->InterruptRequest(InterruptController::IRQ::CDROM);
}

TickCount CDROM::GetAckDelayForCommand(Command command)
{
  if (command == Command::Init)
  {
    // Init takes longer.
    return 120000;
  }

  // Tests show that the average time to acknowledge a command is significantly higher when a disc is in the drive,
  // presumably because the controller is busy doing discy-things.
  constexpr u32 default_ack_delay_no_disc = 15000;
  constexpr u32 default_ack_delay_with_disc = 25000;
  return CanReadMedia() ? default_ack_delay_with_disc : default_ack_delay_no_disc;
}

TickCount CDROM::GetTicksForRead()
{
  return m_mode.double_speed ? (MASTER_CLOCK / 150) : (MASTER_CLOCK / 75);
}

TickCount CDROM::GetTicksForSeek(CDImage::LBA new_lba)
{
  const CDImage::LBA current_lba = m_secondary_status.motor_on ? m_current_lba : 0;
  const u32 lba_diff = static_cast<u32>((new_lba > current_lba) ? (new_lba - current_lba) : (current_lba - new_lba));

  // Formula from Mednafen.
  TickCount ticks = std::max<TickCount>(
    20000, static_cast<u32>(
             ((static_cast<u64>(lba_diff) * static_cast<u64>(MASTER_CLOCK) * static_cast<u64>(1000)) / (72 * 60 * 75)) /
             1000));
  if (!m_secondary_status.motor_on)
    ticks += MASTER_CLOCK;
  if (lba_diff >= 2550)
    ticks += static_cast<TickCount>(u64(MASTER_CLOCK) * 300 / 1000);
  else if (m_drive_state == DriveState::Idle) // paused
  {
    // When paused, the CDC seems to keep reading the disc until it hits the position it's set to, then skip 10-15
    // sectors back (depending on how far into the disc it is). We'll be generous and use 4 sectors, since on average
    // it's probably closer.
    ticks += GetTicksForRead() * 4u;
  }

  if (m_mode.double_speed != m_current_double_speed)
  {
    Log_DevPrintf("Switched from %s to %s speed", m_current_double_speed ? "double" : "single",
                  m_mode.double_speed ? "double" : "single");
    m_current_double_speed = m_mode.double_speed;

    // Approximate time for the motor to change speed?
    ticks += static_cast<u32>(static_cast<double>(MASTER_CLOCK) * 0.1);
  }

  Log_DevPrintf("Seek time for %u LBAs: %d", lba_diff, ticks);
  return ticks;
}

TickCount CDROM::GetTicksForStop(bool motor_was_on)
{
  return motor_was_on ? (m_mode.double_speed ? 25000000 : 13000000) : 7000;
}

CDImage::LBA CDROM::GetNextSectorToBeRead()
{
  if (!IsReadingOrPlaying())
    return m_current_lba;

  m_reader.WaitForReadToComplete();
  return m_reader.GetLastReadSector();
}

void CDROM::BeginCommand(Command command)
{
  TickCount ack_delay = GetAckDelayForCommand(command);

  if (HasPendingCommand())
  {
    // The behavior here is kinda.. interesting. Some commands seem to take precedence over others, for example
    // sending a Nop command followed by a GetlocP will return the GetlocP response, and the same for the inverse.
    // However, other combinations result in strange behavior, for example sending a Setloc followed by a ReadN will
    // fail with ERROR_REASON_INCORRECT_NUMBER_OF_PARAMETERS. This particular example happens in Voice Idol
    // Collection - Pool Bar Story, and the loading time is lengthened as well as audio slowing down if this
    // behavior is not correct. So, let's use a heuristic; if the number of parameters of the "old" command is
    // greater than the "new" command, empty the FIFO, which will return the error when the command executes.
    // Otherwise, override the command with the new one.
    if (s_command_info[static_cast<u8>(m_command)].expected_parameters >
        s_command_info[static_cast<u8>(command)].expected_parameters)
    {
      Log_WarningPrintf("Ignoring command 0x%02X (%s) and emptying FIFO as 0x%02x (%s) is still pending",
                        static_cast<u8>(command), s_command_info[static_cast<u8>(command)].name,
                        static_cast<u8>(m_command), s_command_info[static_cast<u8>(m_command)].name);
      m_param_fifo.Clear();
      return;
    }

    Log_WarningPrintf("Cancelling pending command 0x%02X (%s) for new command 0x%02X (%s)", static_cast<u8>(m_command),
                      s_command_info[static_cast<u8>(m_command)].name, static_cast<u8>(command),
                      s_command_info[static_cast<u8>(command)].name);

    // subtract the currently-elapsed ack ticks from the new command
    const TickCount elapsed_ticks = m_command_event->GetInterval() - m_command_event->GetTicksUntilNextExecution();
    ack_delay = std::max(ack_delay - elapsed_ticks, 1);
    m_command_event->Deactivate();
  }

  m_command = command;
  m_command_event->SetIntervalAndSchedule(ack_delay);
  UpdateCommandEvent();
  UpdateStatusRegister();
}

void CDROM::EndCommand()
{
  m_param_fifo.Clear();

  m_command = Command::None;
  m_command_event->Deactivate();
  UpdateStatusRegister();
}

void CDROM::AbortCommand()
{
  m_command = Command::None;
  m_command_event->Deactivate();
  UpdateStatusRegister();
}

void CDROM::ExecuteCommand()
{
  const CommandInfo& ci = s_command_info[static_cast<u8>(m_command)];
  Log_DevPrintf("CDROM executing command 0x%02X (%s)", static_cast<u8>(m_command), ci.name);
  if (m_param_fifo.GetSize() < ci.expected_parameters)
  {
    Log_WarningPrintf("Too few parameters for command 0x%02X (%s), expecting %u got %u", static_cast<u8>(m_command),
                      ci.name, ci.expected_parameters, m_param_fifo.GetSize());
    SendErrorResponse(STAT_ERROR, ERROR_REASON_INCORRECT_NUMBER_OF_PARAMETERS);
    EndCommand();
    return;
  }

  if (!m_response_fifo.IsEmpty())
  {
    Log_DebugPrintf("Response FIFO not empty on command begin");
    m_response_fifo.Clear();
  }

  switch (m_command)
  {
    case Command::Getstat:
    {
      Log_DebugPrintf("CDROM Getstat command");

      // if bit 0 or 2 is set, send an additional byte
      SendACKAndStat();

      // shell open bit is cleared after sending the status
      if (CanReadMedia())
        m_secondary_status.shell_open = false;

      EndCommand();
      return;
    }

    case Command::Test:
    {
      const u8 subcommand = m_param_fifo.Pop();
      ExecuteTestCommand(subcommand);
      return;
    }

    case Command::GetID:
    {
      Log_DebugPrintf("CDROM GetID command");
      SendACKAndStat();

      m_drive_state = DriveState::ReadingID;
      m_drive_event->Schedule(GetTicksForSeek(0) + GetTicksForRead());

      EndCommand();
      return;
    }

    case Command::ReadTOC:
    {
      Log_DebugPrintf("CDROM ReadTOC command");
      if (!CanReadMedia())
      {
        SendErrorResponse(STAT_ERROR, 0x80);
      }
      else
      {
        SendACKAndStat();

        m_drive_state = DriveState::ReadingTOC;
        m_drive_event->Schedule(MASTER_CLOCK / 2); // half a second
      }

      EndCommand();
      return;
    }

    case Command::Setfilter:
    {
      const u8 file = m_param_fifo.Peek(0);
      const u8 channel = m_param_fifo.Peek(1);
      Log_DebugPrintf("CDROM setfilter command 0x%02X 0x%02X", ZeroExtend32(file), ZeroExtend32(channel));
      m_xa_filter_file_number = file;
      m_xa_filter_channel_number = channel;
      m_xa_current_set = false;
      SendACKAndStat();
      EndCommand();
      return;
    }

    case Command::Setmode:
    {
      const u8 mode = m_param_fifo.Peek(0);
      Log_DebugPrintf("CDROM setmode command 0x%02X", ZeroExtend32(mode));

      m_mode.bits = mode;
      SendACKAndStat();
      EndCommand();
      return;
    }

    case Command::Setloc:
    {
      m_setloc_position.minute = PackedBCDToBinary(m_param_fifo.Peek(0));
      m_setloc_position.second = PackedBCDToBinary(m_param_fifo.Peek(1));
      m_setloc_position.frame = PackedBCDToBinary(m_param_fifo.Peek(2));
      m_setloc_pending = true;
      Log_DebugPrintf("CDROM setloc command (%02X, %02X, %02X)", ZeroExtend32(m_param_fifo.Peek(0)),
                      ZeroExtend32(m_param_fifo.Peek(1)), ZeroExtend32(m_param_fifo.Peek(2)));
      SendACKAndStat();
      EndCommand();
      return;
    }

    case Command::SeekL:
    case Command::SeekP:
    {
      const bool logical = (m_command == Command::SeekL);
      Log_DebugPrintf("CDROM %s command", logical ? "SeekL" : "SeekP");

      if (IsSeeking())
        UpdatePositionWhileSeeking();

      if (!CanReadMedia())
      {
        SendErrorResponse(STAT_ERROR, 0x80);
      }
      else
      {
        SendACKAndStat();
        BeginSeeking(logical, false, false);
      }

      EndCommand();
      return;
    }

    case Command::SetSession:
    {
      const u8 session = m_param_fifo.Peek(0);
      Log_DebugPrintf("CDROM SetSession command, session=%u", session);

      if (!CanReadMedia() || m_drive_state == DriveState::Reading || m_drive_state == DriveState::Playing)
      {
        SendErrorResponse(STAT_ERROR, 0x80);
      }
      else if (session == 0)
      {
        SendErrorResponse(STAT_ERROR, 0x10);
      }
      else
      {
        SendACKAndStat();

        m_async_command_parameter = session;
        m_drive_state = DriveState::ChangingSession;
        m_drive_event->Schedule(MASTER_CLOCK / 2); // half a second
      }

      EndCommand();
      return;
    }

    case Command::ReadN:
    case Command::ReadS:
    {
      Log_DebugPrintf("CDROM read command");
      if (!CanReadMedia())
      {
        SendErrorResponse(STAT_ERROR, 0x80);
      }
      else
      {
        SendACKAndStat();

        if ((!m_setloc_pending || m_setloc_position.ToLBA() == GetNextSectorToBeRead()) &&
            (m_drive_state == DriveState::Reading || (IsSeeking() && m_read_after_seek)))
        {
          Log_DevPrintf("Ignoring read command with no/same setloc, already reading/reading after seek");
        }
        else
        {
          if (IsSeeking())
            UpdatePositionWhileSeeking();

          BeginReading();
        }
      }

      EndCommand();
      return;
    }

    case Command::Play:
    {
      u8 track = m_param_fifo.IsEmpty() ? 0 : m_param_fifo.Peek(0);
      Log_DebugPrintf("CDROM play command, track=%u", track);

      if (!CanReadMedia())
      {
        SendErrorResponse(STAT_ERROR, 0x80);
      }
      else
      {
        SendACKAndStat();

        if (track == 0 && (!m_setloc_pending || m_setloc_position.ToLBA() == GetNextSectorToBeRead()) &&
            (m_drive_state == DriveState::Playing || (IsSeeking() && m_play_after_seek)))
        {
          Log_DevPrintf("Ignoring play command with no/same setloc, already playing/playing after seek");
        }
        else
        {
          if (IsSeeking())
            UpdatePositionWhileSeeking();

          BeginPlaying(track);
        }
      }

      EndCommand();
      return;
    }

    case Command::Pause:
    {
      if (m_secondary_status.seeking)
      {
        Log_WarningPrintf("CDROM Pause command while seeking - sending error response");
        SendErrorResponse(STAT_ERROR, ERROR_REASON_NOT_READY);
        EndCommand();
        return;
      }

      // TODO: Should return an error if seeking.
      const bool was_reading = (m_drive_state == DriveState::Reading || m_drive_state == DriveState::Playing);
      const TickCount pause_time = was_reading ? (m_mode.double_speed ? 2000000 : 1000000) : 7000;
      Log_DebugPrintf("CDROM pause command");
      SendACKAndStat();

      m_drive_state = DriveState::Pausing;
      m_drive_event->Schedule(pause_time);

      EndCommand();
      return;
    }

    case Command::Stop:
    {
      const TickCount stop_time = GetTicksForStop(m_secondary_status.motor_on);
      Log_DebugPrintf("CDROM stop command");
      SendACKAndStat();

      m_drive_state = DriveState::Stopping;
      m_drive_event->Schedule(stop_time);

      EndCommand();
      return;
    }

    case Command::Reset:
    {
      Log_DebugPrintf("CDROM reset command");
      SendACKAndStat();

      if (IsSeeking())
        UpdatePositionWhileSeeking();

      m_drive_state = DriveState::Resetting;
      m_drive_event->SetIntervalAndSchedule(BASE_RESET_TICKS + GetTicksForSeek(0));
      m_seek_start_lba = m_current_lba;
      m_seek_end_lba = 0;

      EndCommand();
      return;
    }
    break;

    case Command::MotorOn:
    {
      Log_DebugPrintf("CDROM motor on command");
      if (m_secondary_status.motor_on)
      {
        SendErrorResponse(STAT_ERROR, 0x20);
      }
      else
      {
        SendACKAndStat();

        m_drive_state = DriveState::Resetting;
        m_drive_event->Schedule(MASTER_CLOCK);
      }

      EndCommand();
      return;
    }
    break;

    case Command::Mute:
    {
      Log_DebugPrintf("CDROM mute command");
      m_muted = true;
      SendACKAndStat();
      EndCommand();
    }
    break;

    case Command::Demute:
    {
      Log_DebugPrintf("CDROM demute command");
      m_muted = false;
      SendACKAndStat();
      EndCommand();
    }
    break;

    case Command::GetlocL:
    {
      if (!m_last_sector_header_valid)
      {
        Log_DevPrintf("CDROM GetlocL command - header invalid, status 0x%02X", m_secondary_status.bits);
        SendErrorResponse(STAT_ERROR, ERROR_REASON_NOT_READY);
      }
      else
      {
        Log_DebugPrintf("CDROM GetlocL command - [%02X:%02X:%02X]", m_last_sector_header.minute,
                        m_last_sector_header.second, m_last_sector_header.frame);

        m_response_fifo.PushRange(reinterpret_cast<const u8*>(&m_last_sector_header), sizeof(m_last_sector_header));
        m_response_fifo.PushRange(reinterpret_cast<const u8*>(&m_last_sector_subheader),
                                  sizeof(m_last_sector_subheader));
        SetInterrupt(Interrupt::ACK);
      }

      EndCommand();
      return;
    }

    case Command::GetlocP:
    {
      if (!CanReadMedia())
      {
        Log_DebugPrintf("CDROM GetlocP command - not ready");
        SendErrorResponse(STAT_ERROR, ERROR_REASON_NOT_READY);
      }
      else
      {
        if (IsSeeking())
          UpdatePositionWhileSeeking();

        Log_DebugPrintf("CDROM GetlocP command - T%02x I%02x R[%02x:%02x:%02x] A[%02x:%02x:%02x]",
                        m_last_subq.track_number_bcd, m_last_subq.index_number_bcd, m_last_subq.relative_minute_bcd,
                        m_last_subq.relative_second_bcd, m_last_subq.relative_frame_bcd,
                        m_last_subq.absolute_minute_bcd, m_last_subq.absolute_second_bcd,
                        m_last_subq.absolute_frame_bcd);

        m_response_fifo.Push(m_last_subq.track_number_bcd);
        m_response_fifo.Push(m_last_subq.index_number_bcd);
        m_response_fifo.Push(m_last_subq.relative_minute_bcd);
        m_response_fifo.Push(m_last_subq.relative_second_bcd);
        m_response_fifo.Push(m_last_subq.relative_frame_bcd);
        m_response_fifo.Push(m_last_subq.absolute_minute_bcd);
        m_response_fifo.Push(m_last_subq.absolute_second_bcd);
        m_response_fifo.Push(m_last_subq.absolute_frame_bcd);
        SetInterrupt(Interrupt::ACK);
      }

      EndCommand();
      return;
    }

    case Command::GetTN:
    {
      Log_DebugPrintf("CDROM GetTN command");
      if (CanReadMedia())
      {
        m_reader.WaitForReadToComplete();

        m_response_fifo.Push(m_secondary_status.bits);
        m_response_fifo.Push(BinaryToBCD(Truncate8(m_reader.GetMedia()->GetFirstTrackNumber())));
        m_response_fifo.Push(BinaryToBCD(Truncate8(m_reader.GetMedia()->GetLastTrackNumber())));
        SetInterrupt(Interrupt::ACK);
      }
      else
      {
        SendErrorResponse(STAT_ERROR, 0x80);
      }

      EndCommand();
    }
    break;

    case Command::GetTD:
    {
      Log_DebugPrintf("CDROM GetTD command");
      Assert(m_param_fifo.GetSize() >= 1);
      const u8 track = PackedBCDToBinary(m_param_fifo.Peek());

      if (!CanReadMedia())
      {
        SendErrorResponse(STAT_ERROR, 0x80);
      }
      else if (track > m_reader.GetMedia()->GetTrackCount())
      {
        SendErrorResponse(STAT_ERROR, 0x10);
      }
      else
      {
        CDImage::Position pos;
        if (track == 0)
          pos = CDImage::Position::FromLBA(m_reader.GetMedia()->GetLBACount());
        else
          pos = m_reader.GetMedia()->GetTrackStartMSFPosition(track);

        m_response_fifo.Push(m_secondary_status.bits);
        m_response_fifo.Push(BinaryToBCD(Truncate8(pos.minute)));
        m_response_fifo.Push(BinaryToBCD(Truncate8(pos.second)));
        SetInterrupt(Interrupt::ACK);
      }

      EndCommand();
    }
    break;

    case Command::Getparam:
    {
      Log_DebugPrintf("CDROM Getparam command");

      m_response_fifo.Push(m_secondary_status.bits);
      m_response_fifo.Push(m_mode.bits);
      m_response_fifo.Push(0);
      m_response_fifo.Push(m_xa_filter_file_number);
      m_response_fifo.Push(m_xa_filter_channel_number);
      SetInterrupt(Interrupt::ACK);
      EndCommand();
    }
    break;

    case Command::Sync:
    {
      Log_DebugPrintf("CDROM sync command");

      SendErrorResponse(STAT_ERROR, 0x40);
      EndCommand();
    }
    break;

    default:
    {
      Log_ErrorPrintf("Unknown CDROM command 0x%04X with %u parameters, please report", static_cast<u16>(m_command),
                      m_param_fifo.GetSize());
      Panic("Unknown CDROM command");
    }
    break;
  }
}

void CDROM::ExecuteTestCommand(u8 subcommand)
{
  switch (subcommand)
  {
    case 0x04: // Reset SCEx counters
    {
      Log_DebugPrintf("Reset SCEx counters");
      m_secondary_status.motor_on = true;
      m_response_fifo.Push(m_secondary_status.bits);
      SetInterrupt(Interrupt::ACK);
      EndCommand();
      return;
    }

    case 0x05: // Read SCEx counters
    {
      Log_DebugPrintf("Read SCEx counters");
      m_response_fifo.Push(m_secondary_status.bits);
      m_response_fifo.Push(0); // # of TOC reads?
      m_response_fifo.Push(0); // # of SCEx strings received
      SetInterrupt(Interrupt::ACK);
      EndCommand();
      return;
    }

    case 0x20: // Get CDROM BIOS Date/Version
    {
      Log_DebugPrintf("Get CDROM BIOS Date/Version");

      static constexpr u8 response[] = {0x95, 0x05, 0x16, 0xC1};
      m_response_fifo.PushRange(response, countof(response));
      SetInterrupt(Interrupt::ACK);
      EndCommand();
      return;
    }

    case 0x22:
    {
      Log_DebugPrintf("Get CDROM region ID string");

      switch (m_system->GetRegion())
      {
        case ConsoleRegion::NTSC_J:
        {
          static constexpr u8 response[] = {'f', 'o', 'r', ' ', 'J', 'a', 'p', 'a', 'n'};
          m_response_fifo.PushRange(response, countof(response));
        }
        break;

        case ConsoleRegion::PAL:
        {
          static constexpr u8 response[] = {'f', 'o', 'r', ' ', 'E', 'u', 'r', 'o', 'p', 'e'};
          m_response_fifo.PushRange(response, countof(response));
        }
        break;

        case ConsoleRegion::NTSC_U:
        default:
        {
          static constexpr u8 response[] = {'f', 'o', 'r', ' ', 'U', '/', 'C'};
          m_response_fifo.PushRange(response, countof(response));
        }
        break;
      }

      SetInterrupt(Interrupt::ACK);
      EndCommand();
      return;
    }

    default:
    {
      Log_ErrorPrintf("Unknown test command 0x%02X, %u parameters", subcommand, m_param_fifo.GetSize());
      Panic("Unknown test command");
      return;
    }
  }
}

void CDROM::UpdateCommandEvent()
{
  // if there's a pending interrupt, we can't execute the command yet
  // so deactivate it until the interrupt is acknowledged
  if (!HasPendingCommand() || HasPendingInterrupt())
  {
    m_command_event->Deactivate();
    return;
  }
  else if (HasPendingCommand())
  {
    m_command_event->Activate();
  }
}

void CDROM::ExecuteDrive(TickCount ticks_late)
{
  switch (m_drive_state)
  {
    case DriveState::Resetting:
      DoResetComplete(ticks_late);
      break;

    case DriveState::ShellOpening:
      DoShellOpenComplete(ticks_late);
      break;

    case DriveState::SeekingPhysical:
    case DriveState::SeekingLogical:
      DoSeekComplete(ticks_late);
      break;

    case DriveState::Pausing:
      DoPauseComplete();
      break;

    case DriveState::Stopping:
      DoStopComplete();
      break;

    case DriveState::ReadingID:
      DoIDRead();
      break;

    case DriveState::ReadingTOC:
      DoTOCRead();
      break;

    case DriveState::Reading:
    case DriveState::Playing:
      DoSectorRead();
      break;

    case DriveState::ChangingSession:
      DoChangeSessionComplete();
      break;

    case DriveState::Idle:
    default:
      break;
  }
}

void CDROM::BeginReading(TickCount ticks_late /* = 0 */, bool after_seek /* = false */)
{
  ClearSectorBuffers();

  if (!after_seek && m_setloc_pending)
  {
    BeginSeeking(true, true, false);
    return;
  }

  // If we were seeking, we want to start reading from the seek target, not the current sector
  // Fixes crash in Disney's The Lion King - Simba's Mighty Adventure.
  if (IsSeeking())
  {
    Log_DevPrintf("Read command while seeking, scheduling read after seek %u -> %u finishes in %d ticks",
                  m_seek_start_lba, m_seek_end_lba, m_drive_event->GetTicksUntilNextExecution());
    m_read_after_seek = true;
    m_play_after_seek = false;
    return;
  }

  Log_DebugPrintf("Starting reading @ LBA %u", m_current_lba);
  m_secondary_status.ClearActiveBits();
  m_secondary_status.motor_on = true;

  const TickCount ticks = GetTicksForRead();
  const TickCount first_sector_ticks = ticks + (after_seek ? 0 : GetTicksForSeek(m_current_lba)) - ticks_late;
  m_drive_state = DriveState::Reading;
  m_drive_event->SetInterval(ticks);
  m_drive_event->Schedule(first_sector_ticks);
  m_current_read_sector_buffer = 0;
  m_current_write_sector_buffer = 0;
  ResetCurrentXAFile();
  ResetXAResampler();

  m_reader.QueueReadSector(m_current_lba);
}

void CDROM::BeginPlaying(u8 track_bcd, TickCount ticks_late /* = 0 */, bool after_seek /* = false */)
{
  Log_DebugPrintf("Starting playing CDDA track %x", track_bcd);
  m_last_cdda_report_frame_nibble = 0xFF;
  m_play_track_number_bcd = track_bcd;

  // if track zero, start from current position
  if (track_bcd != 0)
  {
    // play specific track?
    if (track_bcd > m_reader.GetMedia()->GetTrackCount())
    {
      // restart current track
      track_bcd = BinaryToBCD(Truncate8(m_reader.GetMedia()->GetTrackNumber()));
    }

    m_setloc_position = m_reader.GetMedia()->GetTrackStartMSFPosition(PackedBCDToBinary(track_bcd));
    m_setloc_pending = true;
  }

  if (m_setloc_pending)
  {
    BeginSeeking(false, false, true);
    return;
  }

  m_secondary_status.ClearActiveBits();
  m_secondary_status.motor_on = true;
  ClearSectorBuffers();

  const TickCount ticks = GetTicksForRead();
  const TickCount first_sector_ticks = ticks + (after_seek ? 0 : GetTicksForSeek(m_current_lba)) - ticks_late;
  m_drive_state = DriveState::Playing;
  m_drive_event->SetInterval(ticks);
  m_drive_event->Schedule(first_sector_ticks);
  m_current_read_sector_buffer = 0;
  m_current_write_sector_buffer = 0;

  m_reader.QueueReadSector(m_current_lba);
}

void CDROM::BeginSeeking(bool logical, bool read_after_seek, bool play_after_seek)
{
  if (!m_setloc_pending)
    Log_WarningPrintf("Seeking without setloc set");

  m_read_after_seek = read_after_seek;
  m_play_after_seek = play_after_seek;
  m_setloc_pending = false;

  Log_DebugPrintf("Seeking to [%02u:%02u:%02u] (LBA %u) (%s)", m_setloc_position.minute, m_setloc_position.second,
                  m_setloc_position.frame, m_setloc_position.ToLBA(), logical ? "logical" : "physical");

  const CDImage::LBA seek_lba = m_setloc_position.ToLBA();
  const TickCount seek_time = GetTicksForSeek(seek_lba);

  m_secondary_status.ClearActiveBits();
  m_secondary_status.motor_on = true;
  m_secondary_status.seeking = true;
  m_last_sector_header_valid = false;

  m_drive_state = logical ? DriveState::SeekingLogical : DriveState::SeekingPhysical;
  m_drive_event->SetIntervalAndSchedule(seek_time);

  m_seek_start_lba = m_current_lba;
  m_seek_end_lba = seek_lba;
  m_reader.QueueReadSector(seek_lba);
}

void CDROM::UpdatePositionWhileSeeking()
{
  DebugAssert(IsSeeking());

  const float completed_frac = 1.0f - (static_cast<float>(m_drive_event->GetTicksUntilNextExecution()) /
                                       static_cast<float>(m_drive_event->GetInterval()));

  CDImage::LBA current_lba;
  if (m_seek_end_lba > m_seek_start_lba)
  {
    current_lba = m_seek_start_lba +
                  static_cast<CDImage::LBA>(static_cast<float>(m_seek_end_lba - m_seek_start_lba) * completed_frac);
  }
  else
  {
    current_lba = m_seek_start_lba -
                  static_cast<CDImage::LBA>(static_cast<float>(m_seek_start_lba - m_seek_end_lba) * completed_frac);
  }

  Log_DevPrintf("Update position while seeking from %u to %u - %u (%.2f)", m_seek_start_lba, m_seek_end_lba,
                current_lba, completed_frac);

  // access the image directly since we want to preserve the cached data for the seek complete
  CDImage::SubChannelQ subq;
  if (m_reader.ReadSectorUncached(current_lba, &subq, nullptr) && subq.IsCRCValid())
  {
    m_last_subq = subq;
    m_current_lba = current_lba;
  }
}

void CDROM::DoShellOpenComplete(TickCount ticks_late)
{
  // media is now readable (if any)
  m_drive_state = DriveState::Idle;
  m_drive_event->Deactivate();

  if (m_reader.HasMedia())
    m_secondary_status.motor_on = true;
}

void CDROM::DoResetComplete(TickCount ticks_late)
{
  m_drive_state = DriveState::Idle;
  m_drive_event->Deactivate();

  m_secondary_status.bits = 0;
  m_secondary_status.motor_on = CanReadMedia();
  m_mode.bits = 0;
  m_mode.read_raw_sector = true;

  if (!CanReadMedia())
  {
    Log_DevPrintf("CDROM reset - no disc");
    m_secondary_status.shell_open = true;
    SendAsyncErrorResponse(STAT_ERROR, 0x08);
    return;
  }

  m_current_lba = 0;
  m_reader.QueueReadSector(0);

  m_async_response_fifo.Clear();
  m_async_response_fifo.Push(m_secondary_status.bits);
  SetAsyncInterrupt(Interrupt::Complete);

  if (!CanReadMedia())
  {
    m_secondary_status.motor_on = false;
    m_secondary_status.shell_open = true;
  }
}

void CDROM::DoSeekComplete(TickCount ticks_late)
{
  const bool logical = (m_drive_state == DriveState::SeekingLogical);
  m_drive_state = DriveState::Idle;
  m_drive_event->Deactivate();
  m_secondary_status.ClearActiveBits();

  bool seek_okay = m_reader.WaitForReadToComplete();
  if (seek_okay)
  {
    const CDImage::SubChannelQ& subq = m_reader.GetSectorSubQ();
    if (subq.IsCRCValid())
    {
      // seek and update sub-q for ReadP command
      m_last_subq = subq;
      const auto [seek_mm, seek_ss, seek_ff] = CDImage::Position::FromLBA(m_reader.GetLastReadSector()).ToBCD();
      seek_okay = (subq.IsCRCValid() && subq.absolute_minute_bcd == seek_mm && subq.absolute_second_bcd == seek_ss &&
                   subq.absolute_frame_bcd == seek_ff);
      if (seek_okay)
      {
        if (subq.control.data && logical)
        {
          // ensure the location matches up (it should)
          ProcessDataSectorHeader(m_reader.GetSectorBuffer().data());
          seek_okay = (m_last_sector_header.minute == seek_mm && m_last_sector_header.second == seek_ss &&
                       m_last_sector_header.frame == seek_ff);
        }
        else
        {
          if (logical)
          {
            Log_WarningPrintf("Logical seek to non-data sector [%02x:%02x:%02x]", seek_mm, seek_ss, seek_ff);
            seek_okay = false;
          }
        }

        if (subq.track_number_bcd == CDImage::LEAD_OUT_TRACK_NUMBER)
        {
          Log_WarningPrintf("Invalid seek to lead-out area (LBA %u)", m_reader.GetLastReadSector());
          seek_okay = false;
        }
      }
    }
  }

  if (seek_okay)
  {
    m_current_lba = m_reader.GetLastReadSector();

    // seek complete, transition to play/read if requested
    // INT2 is not sent on play/read
    if (m_read_after_seek)
    {
      BeginReading(ticks_late, true);
    }
    else if (m_play_after_seek)
    {
      BeginPlaying(0, ticks_late, true);
    }
    else
    {
      m_async_response_fifo.Push(m_secondary_status.bits);
      SetAsyncInterrupt(Interrupt::Complete);
    }
  }
  else
  {
    CDImage::Position pos(CDImage::Position::FromLBA(m_reader.GetLastReadSector()));
    Log_WarningPrintf("%s seek to [%02u:%02u:%02u] failed", logical ? "Logical" : "Physical", pos.minute, pos.second,
                      pos.frame);
    SendAsyncErrorResponse(STAT_SEEK_ERROR, 0x04);
    m_last_sector_header_valid = false;
  }

  m_setloc_pending = false;
  m_read_after_seek = false;
  m_play_after_seek = false;
  UpdateStatusRegister();
}

void CDROM::DoPauseComplete()
{
  Log_DebugPrintf("Pause complete");
  m_drive_state = DriveState::Idle;
  m_drive_event->Deactivate();
  m_secondary_status.ClearActiveBits();

  m_async_response_fifo.Clear();
  m_async_response_fifo.Push(m_secondary_status.bits);
  SetAsyncInterrupt(Interrupt::Complete);
}

void CDROM::DoStopComplete()
{
  Log_DebugPrintf("Stop complete");
  m_drive_state = DriveState::Idle;
  m_drive_event->Deactivate();
  m_secondary_status.ClearActiveBits();
  m_secondary_status.motor_on = false;

  m_async_response_fifo.Clear();
  m_async_response_fifo.Push(m_secondary_status.bits);
  SetAsyncInterrupt(Interrupt::Complete);
}

void CDROM::DoChangeSessionComplete()
{
  Log_DebugPrintf("Changing session complete");
  m_drive_state = DriveState::Idle;
  m_drive_event->Deactivate();
  m_secondary_status.ClearActiveBits();
  m_secondary_status.motor_on = true;

  m_async_response_fifo.Clear();
  if (m_async_command_parameter == 0x01)
  {
    m_async_response_fifo.Push(m_secondary_status.bits);
    SetAsyncInterrupt(Interrupt::Complete);
  }
  else
  {
    // we don't emulate multisession discs.. for now
    SendAsyncErrorResponse(STAT_SEEK_ERROR, 0x40);
  }
}

void CDROM::DoIDRead()
{
  Log_DebugPrintf("ID read complete");
  m_drive_state = DriveState::Idle;
  m_drive_event->Deactivate();
  m_secondary_status.ClearActiveBits();
  m_secondary_status.motor_on = CanReadMedia();

  // TODO: Audio CD.
  u8 stat_byte = m_secondary_status.bits;
  u8 flags_byte = 0;
  if (!CanReadMedia())
  {
    flags_byte |= (1 << 6); // Disc Missing
  }
  else
  {
    // this is where it would get read from the start of the disc?
    m_current_lba = 0;
    m_reader.QueueReadSector(0);

    if (m_system->GetSettings().cdrom_region_check &&
        (m_disc_region == DiscRegion::Other ||
         m_system->GetRegion() != System::GetConsoleRegionForDiscRegion(m_disc_region)))
    {
      stat_byte |= STAT_ID_ERROR;
      flags_byte |= (1 << 7); // Unlicensed
    }
  }

  m_async_response_fifo.Clear();
  m_async_response_fifo.Push(stat_byte);
  m_async_response_fifo.Push(flags_byte);
  m_async_response_fifo.Push(0x20); // TODO: Disc type from TOC
  m_async_response_fifo.Push(0x00); // TODO: Session info?

  static constexpr u32 REGION_STRING_LENGTH = 4;
  static constexpr std::array<std::array<u8, REGION_STRING_LENGTH>, static_cast<size_t>(DiscRegion::Count)>
    region_strings = {{{'S', 'C', 'E', 'I'}, {'S', 'C', 'E', 'A'}, {'S', 'C', 'E', 'E'}, {0, 0, 0, 0}}};
  m_async_response_fifo.PushRange(region_strings[static_cast<u8>(m_disc_region)].data(), REGION_STRING_LENGTH);

  SetAsyncInterrupt((flags_byte != 0) ? Interrupt::Error : Interrupt::Complete);
}

void CDROM::DoTOCRead()
{
  Log_DebugPrintf("TOC read complete");
  m_drive_state = DriveState::Idle;
  m_drive_event->Deactivate();

  m_async_response_fifo.Clear();
  m_async_response_fifo.Push(m_secondary_status.bits);
  SetAsyncInterrupt(Interrupt::Complete);
}

void CDROM::StopReadingWithDataEnd()
{
  ClearAsyncInterrupt();
  m_async_response_fifo.Push(m_secondary_status.bits);
  SetAsyncInterrupt(Interrupt::DataEnd);

  m_secondary_status.ClearActiveBits();
  m_drive_state = DriveState::Idle;
  m_drive_event->Deactivate();
}

void CDROM::DoSectorRead()
{
  if (!m_reader.WaitForReadToComplete())
    Panic("Sector read failed");

  // TODO: Queue the next read here and swap the buffer.
  m_current_lba = m_reader.GetLastReadSector();

  // TODO: Error handling
  const CDImage::SubChannelQ& subq = m_reader.GetSectorSubQ();
  if (subq.track_number_bcd == CDImage::LEAD_OUT_TRACK_NUMBER)
  {
    Log_DevPrintf("Read reached lead-out area of disc at LBA %u, pausing", m_reader.GetLastReadSector());
    StopReadingWithDataEnd();
    return;
  }

  const bool is_data_sector = subq.control.data;
  if (!is_data_sector)
  {
    if (m_play_track_number_bcd == 0)
    {
      // track number was not specified, but we've found the track now
      m_play_track_number_bcd = subq.track_number_bcd;
      Log_DebugPrintf("Setting playing track number to %u", m_play_track_number_bcd);
    }
    else if (m_mode.auto_pause && subq.track_number_bcd != m_play_track_number_bcd)
    {
      // we don't want to update the position if the track changes, so we check it before reading the actual sector.
      Log_DevPrintf("Auto pause at the end of track %u (LBA %u)", m_play_track_number_bcd,
                    m_reader.GetLastReadSector());

      StopReadingWithDataEnd();
      return;
    }
  }
  else
  {
    ProcessDataSectorHeader(m_reader.GetSectorBuffer().data());
  }

  if (subq.IsCRCValid())
  {
    m_last_subq = subq;
  }
  else
  {
    const CDImage::Position pos(CDImage::Position::FromLBA(m_current_lba));
    Log_DevPrintf("Sector %u [%02u:%02u:%02u] has invalid subchannel Q", m_current_lba, pos.minute, pos.second,
                  pos.frame);
  }

  if (is_data_sector && m_drive_state == DriveState::Reading)
  {
    ProcessDataSector(m_reader.GetSectorBuffer().data(), subq);
  }
  else if (!is_data_sector && m_drive_state == DriveState::Playing)
  {
    ProcessCDDASector(m_reader.GetSectorBuffer().data(), subq);
  }
  else if (m_drive_state != DriveState::Reading && m_drive_state != DriveState::Playing)
  {
    Panic("Not reading or playing");
  }
  else
  {
    Log_WarningPrintf("Skipping sector %u as it is a %s sector and we're not %s", m_current_lba,
                      is_data_sector ? "data" : "audio", is_data_sector ? "reading" : "playing");
  }

  m_reader.QueueReadSector(m_current_lba + 1u);
}

void CDROM::ProcessDataSectorHeader(const u8* raw_sector)
{
  std::memcpy(&m_last_sector_header, &raw_sector[SECTOR_SYNC_SIZE], sizeof(m_last_sector_header));
  std::memcpy(&m_last_sector_subheader, &raw_sector[SECTOR_SYNC_SIZE + sizeof(m_last_sector_header)],
              sizeof(m_last_sector_subheader));
  m_last_sector_header_valid = true;
}

void CDROM::ProcessDataSector(const u8* raw_sector, const CDImage::SubChannelQ& subq)
{
  Log_DevPrintf("Read sector %u: mode %u submode 0x%02X into buffer %u", m_current_lba,
                ZeroExtend32(m_last_sector_header.sector_mode), ZeroExtend32(m_last_sector_subheader.submode.bits),
                m_current_write_sector_buffer);

  // The reading bit shouldn't be set until the first sector is processed.
  m_secondary_status.reading = true;

  if (m_mode.xa_enable && m_last_sector_header.sector_mode == 2)
  {
    if (m_last_sector_subheader.submode.realtime && m_last_sector_subheader.submode.audio)
    {
      ProcessXAADPCMSector(raw_sector, subq);

      // Audio+realtime sectors aren't delivered to the CPU.
      return;
    }
  }

  // TODO: How does XA relate to this buffering?
  m_current_write_sector_buffer = (m_current_write_sector_buffer + 1) % NUM_SECTOR_BUFFERS;
  SectorBuffer* sb = &m_sector_buffers[m_current_write_sector_buffer];
  if (sb->size > 0)
  {
    Log_DevPrintf("Sector buffer %u was not read, previous sector dropped",
                  (m_current_write_sector_buffer - 1) % NUM_SECTOR_BUFFERS);
  }

  if (m_mode.ignore_bit)
    Log_WarningPrintf("SetMode.4 bit set on read of sector %u", m_current_lba);

  if (m_mode.read_raw_sector)
  {
    std::memcpy(sb->data.data(), raw_sector + SECTOR_SYNC_SIZE, RAW_SECTOR_OUTPUT_SIZE);
    sb->size = RAW_SECTOR_OUTPUT_SIZE;
  }
  else
  {
    // TODO: This should actually depend on the mode...
    Assert(m_last_sector_header.sector_mode == 2);
    std::memcpy(sb->data.data(), raw_sector + CDImage::SECTOR_SYNC_SIZE + 12, DATA_SECTOR_OUTPUT_SIZE);
    sb->size = DATA_SECTOR_OUTPUT_SIZE;
  }

  // Deliver to CPU
  if (HasPendingAsyncInterrupt())
  {
    Log_WarningPrintf("Data interrupt was not delivered");
    ClearAsyncInterrupt();
  }

  m_async_response_fifo.Push(m_secondary_status.bits);
  SetAsyncInterrupt(Interrupt::DataReady);
}

static std::array<std::array<s16, 29>, 7> s_zigzag_table = {
  {{0,      0x0,     0x0,     0x0,    0x0,     -0x0002, 0x000A,  -0x0022, 0x0041, -0x0054,
    0x0034, 0x0009,  -0x010A, 0x0400, -0x0A78, 0x234C,  0x6794,  -0x1780, 0x0BCD, -0x0623,
    0x0350, -0x016D, 0x006B,  0x000A, -0x0010, 0x0011,  -0x0008, 0x0003,  -0x0001},
   {0,       0x0,    0x0,     -0x0002, 0x0,    0x0003,  -0x0013, 0x003C,  -0x004B, 0x00A2,
    -0x00E3, 0x0132, -0x0043, -0x0267, 0x0C9D, 0x74BB,  -0x11B4, 0x09B8,  -0x05BF, 0x0372,
    -0x01A8, 0x00A6, -0x001B, 0x0005,  0x0006, -0x0008, 0x0003,  -0x0001, 0x0},
   {0,      0x0,     -0x0001, 0x0003,  -0x0002, -0x0005, 0x001F,  -0x004A, 0x00B3, -0x0192,
    0x02B1, -0x039E, 0x04F8,  -0x05A6, 0x7939,  -0x05A6, 0x04F8,  -0x039E, 0x02B1, -0x0192,
    0x00B3, -0x004A, 0x001F,  -0x0005, -0x0002, 0x0003,  -0x0001, 0x0,     0x0},
   {0,       -0x0001, 0x0003,  -0x0008, 0x0006, 0x0005,  -0x001B, 0x00A6, -0x01A8, 0x0372,
    -0x05BF, 0x09B8,  -0x11B4, 0x74BB,  0x0C9D, -0x0267, -0x0043, 0x0132, -0x00E3, 0x00A2,
    -0x004B, 0x003C,  -0x0013, 0x0003,  0x0,    -0x0002, 0x0,     0x0,    0x0},
   {-0x0001, 0x0003,  -0x0008, 0x0011,  -0x0010, 0x000A, 0x006B,  -0x016D, 0x0350, -0x0623,
    0x0BCD,  -0x1780, 0x6794,  0x234C,  -0x0A78, 0x0400, -0x010A, 0x0009,  0x0034, -0x0054,
    0x0041,  -0x0022, 0x000A,  -0x0001, 0x0,     0x0001, 0x0,     0x0,     0x0},
   {0x0002,  -0x0008, 0x0010,  -0x0023, 0x002B, 0x001A,  -0x00EB, 0x027B,  -0x0548, 0x0AFA,
    -0x16FA, 0x53E0,  0x3C07,  -0x1249, 0x080E, -0x0347, 0x015B,  -0x0044, -0x0017, 0x0046,
    -0x0023, 0x0011,  -0x0005, 0x0,     0x0,    0x0,     0x0,     0x0,     0x0},
   {-0x0005, 0x0011,  -0x0023, 0x0046, -0x0017, -0x0044, 0x015B,  -0x0347, 0x080E, -0x1249,
    0x3C07,  0x53E0,  -0x16FA, 0x0AFA, -0x0548, 0x027B,  -0x00EB, 0x001A,  0x002B, -0x0023,
    0x0010,  -0x0008, 0x0002,  0x0,    0x0,     0x0,     0x0,     0x0,     0x0}}};

static s16 ZigZagInterpolate(const s16* ringbuf, const s16* table, u8 p)
{
  s32 sum = 0;
  for (u8 i = 0; i < 29; i++)
    sum += (s32(ringbuf[(p - i) & 0x1F]) * s32(table[i])) / 0x8000;

  return static_cast<s16>(std::clamp<s32>(sum, -0x8000, 0x7FFF));
}

static constexpr s32 ApplyVolume(s16 sample, u8 volume)
{
  return s32(sample) * static_cast<s32>(ZeroExtend32(volume)) >> 7;
}

static constexpr s16 SaturateVolume(s32 volume)
{
  return static_cast<s16>(std::clamp<s32>(volume, -0x8000, 0x7FFF));
}

template<bool STEREO, bool SAMPLE_RATE>
static void ResampleXAADPCM(const s16* frames_in, u32 num_frames_in, SPU* spu, s16* left_ringbuf, s16* right_ringbuf,
                            u8* p_ptr, u8* sixstep_ptr, const std::array<std::array<u8, 2>, 2>& volume_matrix)
{
  u8 p = *p_ptr;
  u8 sixstep = *sixstep_ptr;

  spu->EnsureCDAudioSpace(((num_frames_in * 7) / 6) << BoolToUInt8(SAMPLE_RATE));

  for (u32 in_sample_index = 0; in_sample_index < num_frames_in; in_sample_index++)
  {
    const s16 left = *(frames_in++);
    const s16 right = STEREO ? *(frames_in++) : left;

    if constexpr (!STEREO)
    {
      UNREFERENCED_VARIABLE(right);
    }

    for (u32 sample_dup = 0; sample_dup < (SAMPLE_RATE ? 2 : 1); sample_dup++)
    {
      left_ringbuf[p] = left;
      if constexpr (STEREO)
        right_ringbuf[p] = right;
      p = (p + 1) % 32;
      sixstep--;

      if (sixstep == 0)
      {
        sixstep = 6;
        for (u32 j = 0; j < 7; j++)
        {
          const s16 left_interp = ZigZagInterpolate(left_ringbuf, s_zigzag_table[j].data(), p);
          const s16 right_interp = STEREO ? ZigZagInterpolate(right_ringbuf, s_zigzag_table[j].data(), p) : left_interp;

          const s16 left_out = SaturateVolume(ApplyVolume(left_interp, volume_matrix[0][0]) +
                                              ApplyVolume(right_interp, volume_matrix[1][0]));
          const s16 right_out = SaturateVolume(ApplyVolume(left_interp, volume_matrix[0][1]) +
                                               ApplyVolume(right_interp, volume_matrix[1][1]));

          spu->AddCDAudioSample(left_out, right_out);
        }
      }
    }
  }

  *p_ptr = p;
  *sixstep_ptr = sixstep;
}

void CDROM::ResetCurrentXAFile()
{
  m_xa_current_channel_number = 0;
  m_xa_current_file_number = 0;
  m_xa_current_set = false;
}

void CDROM::ResetXAResampler()
{
  m_xa_last_samples.fill(0);
  for (u32 i = 0; i < 2; i++)
  {
    m_xa_resample_ring_buffer[i].fill(0);
    m_xa_resample_p = 0;
    m_xa_resample_sixstep = 6;
  }
}

void CDROM::ProcessXAADPCMSector(const u8* raw_sector, const CDImage::SubChannelQ& subq)
{
  // Check for automatic ADPCM filter.
  if (m_mode.xa_filter && (m_last_sector_subheader.file_number != m_xa_filter_file_number ||
                           m_last_sector_subheader.channel_number != m_xa_filter_channel_number))
  {
    Log_DebugPrintf("Skipping sector due to filter mismatch (expected %u/%u got %u/%u)", m_xa_filter_file_number,
                    m_xa_filter_channel_number, m_last_sector_subheader.file_number,
                    m_last_sector_subheader.channel_number);
    return;
  }

  // Track the current file being played. If this is not set by the filter, it'll be set by the first file/sector which
  // is read. Fixes audio in Tomb Raider III menu.
  if (!m_xa_current_set)
  {
    // Some games (Taxi 2 and Blues Blues) have junk audio sectors with a channel number of 255.
    // We need to skip them otherwise it ends up playing the incorrect file.
    // TODO: Verify with a hardware test.
    if (m_last_sector_subheader.channel_number == 255 && (!m_mode.xa_filter || m_xa_filter_channel_number != 255))
    {
      Log_WarningPrintf("Skipping XA file with file number %u and channel number %u (submode 0x%02X coding 0x%02X)",
                        m_last_sector_subheader.file_number, m_last_sector_subheader.channel_number,
                        m_last_sector_subheader.submode.bits, m_last_sector_subheader.codinginfo.bits);
      return;
    }

    m_xa_current_file_number = m_last_sector_subheader.file_number;
    m_xa_current_channel_number = m_last_sector_subheader.channel_number;
    m_xa_current_set = true;
    ResetXAResampler();
  }
  else if (m_last_sector_subheader.file_number != m_xa_current_file_number ||
           m_last_sector_subheader.channel_number != m_xa_current_channel_number)
  {
    Log_DebugPrintf("Skipping sector due to current file mismatch (expected %u/%u got %u/%u)", m_xa_current_file_number,
                    m_xa_current_channel_number, m_last_sector_subheader.file_number,
                    m_last_sector_subheader.channel_number);
    return;
  }

  // Reset current file on EOF, and play the file in the next sector.
  if (m_last_sector_subheader.submode.eof)
    ResetCurrentXAFile();

  std::array<s16, CDXA::XA_ADPCM_SAMPLES_PER_SECTOR_4BIT> sample_buffer;
  CDXA::DecodeADPCMSector(raw_sector, sample_buffer.data(), m_xa_last_samples.data());

  // Only send to SPU if we're not muted.
  if (m_muted || m_adpcm_muted)
    return;

  m_spu->GeneratePendingSamples();

  if (m_last_sector_subheader.codinginfo.IsStereo())
  {
    const u32 num_samples = m_last_sector_subheader.codinginfo.GetSamplesPerSector() / 2;
    if (m_last_sector_subheader.codinginfo.IsHalfSampleRate())
    {
      ResampleXAADPCM<true, true>(sample_buffer.data(), num_samples, m_spu, m_xa_resample_ring_buffer[0].data(),
                                  m_xa_resample_ring_buffer[1].data(), &m_xa_resample_p, &m_xa_resample_sixstep,
                                  m_cd_audio_volume_matrix);
    }
    else
    {
      ResampleXAADPCM<true, false>(sample_buffer.data(), num_samples, m_spu, m_xa_resample_ring_buffer[0].data(),
                                   m_xa_resample_ring_buffer[1].data(), &m_xa_resample_p, &m_xa_resample_sixstep,
                                   m_cd_audio_volume_matrix);
    }
  }
  else
  {
    const u32 num_samples = m_last_sector_subheader.codinginfo.GetSamplesPerSector();
    if (m_last_sector_subheader.codinginfo.IsHalfSampleRate())
    {
      ResampleXAADPCM<false, true>(sample_buffer.data(), num_samples, m_spu, m_xa_resample_ring_buffer[0].data(),
                                   m_xa_resample_ring_buffer[1].data(), &m_xa_resample_p, &m_xa_resample_sixstep,
                                   m_cd_audio_volume_matrix);
    }
    else
    {
      ResampleXAADPCM<false, false>(sample_buffer.data(), num_samples, m_spu, m_xa_resample_ring_buffer[0].data(),
                                    m_xa_resample_ring_buffer[1].data(), &m_xa_resample_p, &m_xa_resample_sixstep,
                                    m_cd_audio_volume_matrix);
    }
  }
}

void CDROM::ProcessCDDASector(const u8* raw_sector, const CDImage::SubChannelQ& subq)
{
  // For CDDA sectors, the whole sector contains the audio data.
  Log_DevPrintf("Read sector %u as CDDA", m_current_lba);
  m_secondary_status.playing_cdda = true;

  if (m_mode.report_audio)
  {
    const u8 frame_nibble = subq.absolute_frame_bcd >> 4;
    if (m_last_cdda_report_frame_nibble != frame_nibble)
    {
      m_last_cdda_report_frame_nibble = frame_nibble;

      Log_DebugPrintf("CDDA report at track[%02x] index[%02x] rel[%02x:%02x:%02x]", subq.track_number_bcd,
                      subq.index_number_bcd, subq.relative_minute_bcd, subq.relative_second_bcd,
                      subq.relative_frame_bcd);

      ClearAsyncInterrupt();
      m_async_response_fifo.Push(m_secondary_status.bits);
      m_async_response_fifo.Push(subq.track_number_bcd);
      m_async_response_fifo.Push(subq.index_number_bcd);
      if (subq.absolute_frame_bcd & 0x10)
      {
        m_async_response_fifo.Push(subq.relative_minute_bcd);
        m_async_response_fifo.Push(0x80 | subq.relative_second_bcd);
        m_async_response_fifo.Push(subq.relative_frame_bcd);
      }
      else
      {
        m_async_response_fifo.Push(subq.absolute_minute_bcd);
        m_async_response_fifo.Push(subq.absolute_second_bcd);
        m_async_response_fifo.Push(subq.absolute_frame_bcd);
      }

      m_async_response_fifo.Push(0); // peak low
      m_async_response_fifo.Push(0); // peak high
      SetAsyncInterrupt(Interrupt::DataReady);
    }
  }

  // Apply volume when pushing sectors to SPU.
  if (m_muted)
    return;

  m_spu->GeneratePendingSamples();

  constexpr bool is_stereo = true;
  constexpr u32 num_samples = CDImage::RAW_SECTOR_SIZE / sizeof(s16) / (is_stereo ? 2 : 1);
  m_spu->EnsureCDAudioSpace(num_samples);

  const u8* sector_ptr = raw_sector;
  for (u32 i = 0; i < num_samples; i++)
  {
    s16 samp_left, samp_right;
    std::memcpy(&samp_left, sector_ptr, sizeof(samp_left));
    std::memcpy(&samp_right, sector_ptr + sizeof(s16), sizeof(samp_right));
    sector_ptr += sizeof(s16) * 2;

    const s16 left = SaturateVolume(ApplyVolume(samp_left, m_cd_audio_volume_matrix[0][0]) +
                                    ApplyVolume(samp_right, m_cd_audio_volume_matrix[1][0]));
    const s16 right = SaturateVolume(ApplyVolume(samp_left, m_cd_audio_volume_matrix[0][1]) +
                                     ApplyVolume(samp_right, m_cd_audio_volume_matrix[1][1]));
    m_spu->AddCDAudioSample(left, right);
  }
}

void CDROM::LoadDataFIFO()
{
  if (!m_data_fifo.IsEmpty())
  {
    Log_DevPrintf("Load data fifo when not empty");
    return;
  }

  // any data to load?
  SectorBuffer& sb = m_sector_buffers[m_current_read_sector_buffer];
  if (sb.size == 0)
  {
    Log_WarningPrintf("Attempting to load empty sector buffer");
    m_data_fifo.PushRange(sb.data.data(), RAW_SECTOR_OUTPUT_SIZE);
  }
  else
  {
    m_data_fifo.PushRange(sb.data.data(), sb.size);
    sb.size = 0;
  }

  Log_DebugPrintf("Loaded %u bytes to data FIFO from buffer %u", m_data_fifo.GetSize(), m_current_read_sector_buffer);
  m_current_read_sector_buffer = m_current_write_sector_buffer;
}

void CDROM::ClearSectorBuffers()
{
  for (u32 i = 0; i < NUM_SECTOR_BUFFERS; i++)
    m_sector_buffers[i].size = 0;
}

void CDROM::DrawDebugWindow()
{
  static const ImVec4 active_color{1.0f, 1.0f, 1.0f, 1.0f};
  static const ImVec4 inactive_color{0.4f, 0.4f, 0.4f, 1.0f};
  const float framebuffer_scale = ImGui::GetIO().DisplayFramebufferScale.x;

  ImGui::SetNextWindowSize(ImVec2(800.0f * framebuffer_scale, 500.0f * framebuffer_scale), ImGuiCond_FirstUseEver);
  if (!ImGui::Begin("CDROM State", &m_system->GetSettings().debugging.show_cdrom_state))
  {
    ImGui::End();
    return;
  }

  // draw voice states
  if (ImGui::CollapsingHeader("Media", ImGuiTreeNodeFlags_DefaultOpen))
  {
    if (m_reader.HasMedia())
    {
      const CDImage* media = m_reader.GetMedia();
      const auto [disc_minute, disc_second, disc_frame] = media->GetMSFPositionOnDisc();
      const auto [track_minute, track_second, track_frame] = media->GetMSFPositionInTrack();

      ImGui::Text("Filename: %s", media->GetFileName().c_str());
      ImGui::Text("Disc Position: MSF[%02u:%02u:%02u] LBA[%u]", disc_minute, disc_second, disc_frame,
                  media->GetPositionOnDisc());
      ImGui::Text("Track Position: Number[%u] MSF[%02u:%02u:%02u] LBA[%u]", media->GetTrackNumber(), track_minute,
                  track_second, track_frame, media->GetPositionInTrack());
      ImGui::Text("Last Sector: %02X:%02X:%02X (Mode %u)", m_last_sector_header.minute, m_last_sector_header.second,
                  m_last_sector_header.frame, m_last_sector_header.sector_mode);
    }
    else
    {
      ImGui::Text("No media inserted.");
    }
  }

  if (ImGui::CollapsingHeader("Status/Mode", ImGuiTreeNodeFlags_DefaultOpen))
  {
    static constexpr std::array<const char*, 12> drive_state_names = {
      {"Idle", "Opening Shell", "Resetting", "Seeking (Physical)", "Seeking (Logical)", "Reading ID", "Reading TOC",
       "Reading", "Playing", "Pausing", "Stopping", "Changing Session"}};

    ImGui::Columns(3);

    ImGui::Text("Status");
    ImGui::NextColumn();
    ImGui::Text("Secondary Status");
    ImGui::NextColumn();
    ImGui::Text("Mode Status");
    ImGui::NextColumn();

    ImGui::TextColored(m_status.ADPBUSY ? active_color : inactive_color, "ADPBUSY: %s",
                       m_status.ADPBUSY ? "Yes" : "No");
    ImGui::NextColumn();
    ImGui::TextColored(m_secondary_status.error ? active_color : inactive_color, "Error: %s",
                       m_secondary_status.error ? "Yes" : "No");
    ImGui::NextColumn();
    ImGui::TextColored(m_mode.cdda ? active_color : inactive_color, "CDDA: %s", m_mode.cdda ? "Yes" : "No");
    ImGui::NextColumn();

    ImGui::TextColored(m_status.PRMEMPTY ? active_color : inactive_color, "PRMEMPTY: %s",
                       m_status.PRMEMPTY ? "Yes" : "No");
    ImGui::NextColumn();
    ImGui::TextColored(m_secondary_status.motor_on ? active_color : inactive_color, "Motor On: %s",
                       m_secondary_status.motor_on ? "Yes" : "No");
    ImGui::NextColumn();
    ImGui::TextColored(m_mode.auto_pause ? active_color : inactive_color, "Auto Pause: %s",
                       m_mode.auto_pause ? "Yes" : "No");
    ImGui::NextColumn();

    ImGui::TextColored(m_status.PRMWRDY ? active_color : inactive_color, "PRMWRDY: %s",
                       m_status.PRMWRDY ? "Yes" : "No");
    ImGui::NextColumn();
    ImGui::TextColored(m_secondary_status.seek_error ? active_color : inactive_color, "Seek Error: %s",
                       m_secondary_status.seek_error ? "Yes" : "No");
    ImGui::NextColumn();
    ImGui::TextColored(m_mode.report_audio ? active_color : inactive_color, "Report Audio: %s",
                       m_mode.report_audio ? "Yes" : "No");
    ImGui::NextColumn();

    ImGui::TextColored(m_status.RSLRRDY ? active_color : inactive_color, "RSLRRDY: %s",
                       m_status.RSLRRDY ? "Yes" : "No");
    ImGui::NextColumn();
    ImGui::TextColored(m_secondary_status.id_error ? active_color : inactive_color, "ID Error: %s",
                       m_secondary_status.id_error ? "Yes" : "No");
    ImGui::NextColumn();
    ImGui::TextColored(m_mode.xa_filter ? active_color : inactive_color, "XA Filter: %s (File %u Channel %u)",
                       m_mode.xa_filter ? "Yes" : "No", m_xa_filter_file_number, m_xa_filter_channel_number);
    ImGui::NextColumn();

    ImGui::TextColored(m_status.DRQSTS ? active_color : inactive_color, "DRQSTS: %s", m_status.DRQSTS ? "Yes" : "No");
    ImGui::NextColumn();
    ImGui::TextColored(m_secondary_status.shell_open ? active_color : inactive_color, "Shell Open: %s",
                       m_secondary_status.shell_open ? "Yes" : "No");
    ImGui::NextColumn();
    ImGui::TextColored(m_mode.ignore_bit ? active_color : inactive_color, "Ignore Bit: %s",
                       m_mode.ignore_bit ? "Yes" : "No");
    ImGui::NextColumn();

    ImGui::TextColored(m_status.BUSYSTS ? active_color : inactive_color, "BUSYSTS: %s",
                       m_status.BUSYSTS ? "Yes" : "No");
    ImGui::NextColumn();
    ImGui::TextColored(m_secondary_status.reading ? active_color : inactive_color, "Reading: %s",
                       m_secondary_status.reading ? "Yes" : "No");
    ImGui::NextColumn();
    ImGui::TextColored(m_mode.read_raw_sector ? active_color : inactive_color, "Read Raw Sectors: %s",
                       m_mode.read_raw_sector ? "Yes" : "No");
    ImGui::NextColumn();

    ImGui::NextColumn();
    ImGui::TextColored(m_secondary_status.seeking ? active_color : inactive_color, "Seeking: %s",
                       m_secondary_status.seeking ? "Yes" : "No");
    ImGui::NextColumn();
    ImGui::TextColored(m_mode.xa_enable ? active_color : inactive_color, "XA Enable: %s",
                       m_mode.xa_enable ? "Yes" : "No");
    ImGui::NextColumn();

    ImGui::NextColumn();
    ImGui::TextColored(m_secondary_status.playing_cdda ? active_color : inactive_color, "Playing CDDA: %s",
                       m_secondary_status.playing_cdda ? "Yes" : "No");
    ImGui::NextColumn();
    ImGui::TextColored(m_mode.double_speed ? active_color : inactive_color, "Double Speed: %s",
                       m_mode.double_speed ? "Yes" : "No");
    ImGui::NextColumn();

    ImGui::Columns(1);
    ImGui::NewLine();

    if (HasPendingCommand())
    {
      ImGui::TextColored(active_color, "Command: 0x%02X (%d ticks remaining)", static_cast<u8>(m_command),
                         m_command_event->IsActive() ? m_command_event->GetTicksUntilNextExecution() : 0);
    }
    else
    {
      ImGui::TextColored(inactive_color, "Command: None");
    }

    if (IsDriveIdle())
    {
      ImGui::TextColored(inactive_color, "Drive: Idle");
    }
    else
    {
      ImGui::TextColored(active_color, "Drive: %s (%d ticks remaining)",
                         drive_state_names[static_cast<u8>(m_drive_state)],
                         m_drive_event->IsActive() ? m_drive_event->GetTicksUntilNextExecution() : 0);
    }

    ImGui::Text("Interrupt Enable Register: 0x%02X", m_interrupt_enable_register);
    ImGui::Text("Interrupt Flag Register: 0x%02X", m_interrupt_flag_register);
  }

  if (ImGui::CollapsingHeader("CD Audio", ImGuiTreeNodeFlags_DefaultOpen))
  {
    if (m_drive_state == DriveState::Reading && m_mode.xa_enable)
    {
      ImGui::TextColored(active_color, "Playing: XA-ADPCM (File %u / Channel %u)", m_xa_current_file_number,
                         m_xa_current_channel_number);
    }
    else if (m_drive_state == DriveState::Playing)
    {
      ImGui::TextColored(active_color, "Playing: CDDA (Track %x)", m_last_subq.track_number_bcd);
    }
    else
    {
      ImGui::TextColored(inactive_color, "Playing: Inactive");
    }

    ImGui::TextColored(m_muted ? inactive_color : active_color, "Muted: %s", m_muted ? "Yes" : "No");
    ImGui::Text("Left Output: Left Channel=%02X (%u%%), Right Channel=%02X (%u%%)", m_cd_audio_volume_matrix[0][0],
                ZeroExtend32(m_cd_audio_volume_matrix[0][0]) * 100 / 0x80, m_cd_audio_volume_matrix[1][0],
                ZeroExtend32(m_cd_audio_volume_matrix[1][0]) * 100 / 0x80);
    ImGui::Text("Right Output: Left Channel=%02X (%u%%), Right Channel=%02X (%u%%)", m_cd_audio_volume_matrix[0][1],
                ZeroExtend32(m_cd_audio_volume_matrix[0][1]) * 100 / 0x80, m_cd_audio_volume_matrix[1][1],
                ZeroExtend32(m_cd_audio_volume_matrix[1][1]) * 100 / 0x80);
  }

  ImGui::End();
}

from pxr import Usd
import unf


def test_manager_create():
    """Create a single reporter manager per stage."""
    stage = Usd.Stage.CreateInMemory()
    manager1 = unf.ReporterManager.Create(stage)
    assert unf.ReporterManager.GetNumManagers() == 1

    manager2 = unf.ReporterManager.Create(stage)
    assert unf.ReporterManager.GetNumManagers() == 1
    assert manager1 == manager2

    otherStage = Usd.Stage.CreateInMemory()
    manager3 = unf.ReporterManager.Create(otherStage)
    assert unf.ReporterManager.GetNumManagers() == 2
    assert manager1 != manager3

def test_manager_stage_expired():
    """Clean manager when stage is expired."""
    stage1 = Usd.Stage.CreateInMemory()
    manager1 = unf.ReporterManager.Create(stage1)

    # Stage is destroyed, but manager reference is kept in registry
    del stage1
    assert manager1.expired is False
    assert unf.ReporterManager.GetNumManagers() == 1

    # Registry reference is removed when a manager is added
    stage2 = Usd.Stage.CreateInMemory()
    manager2 = unf.ReporterManager.Create(stage2)
    assert manager1.expired is True
    assert manager2.expired is False
    assert unf.ReporterManager.GetNumManagers() == 1

def test_manager_reset():
    """Explicitly reset manager."""
    stage1 = Usd.Stage.CreateInMemory()
    manager1 = unf.ReporterManager.Create(stage1)

    stage2 = Usd.Stage.CreateInMemory()
    manager2 = unf.ReporterManager.Create(stage2)

    # Explicitly reset one manager
    manager1.Reset()
    assert manager1.expired is True
    assert manager2.expired is False

def test_manager_reset_all():
    """Explicitly reset all managers."""
    stage1 = Usd.Stage.CreateInMemory()
    manager1 = unf.ReporterManager.Create(stage1)

    stage2 = Usd.Stage.CreateInMemory()
    manager2 = unf.ReporterManager.Create(stage2)

    # Explicitly reset one manager
    unf.ReporterManager.ResetAll()
    assert manager1.expired is True
    assert manager2.expired is True
